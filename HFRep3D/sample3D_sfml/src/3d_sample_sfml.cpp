#include <cmath>
#include <string.h>
#include <vector>
#include <functional>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SFMLwindow.h"
#include "camera.h"
#include "volumetricObj.h"
#include "frep3D.h"
#include "skybox.h"
#include "hfrep3D.h"

using namespace std::placeholders;

namespace args
{
    int width  = 0;
    int height = 0;

    std::vector<std::string> skyBoxTex = { "textures/right.jpg",
                                           "textures/left.jpg",
                                           "textures/top.jpg",
                                           "textures/bottom.jpg",
                                           "textures/back.jpg",
                                           "textures/front.jpg"
                                         };
}

bool consoleInput( int argc, char** argv )
{
    for ( int i = 0; i < argc; i++)
    {
        const char *cur = argv[i];
        if( !strcmp( cur, "-w" ))
            args::width = std::stoi( argv[i+1] );
        if( !strcmp( cur, "-h" ))
            args::height = std::stoi( argv[i+1] );
        else {
            std::cout << "Warning: default values for width and height of the window will be used! " << std::endl;
            std::cout << " width [-w] = 1366, height [-h] = 768 " << std::endl;
            return false;
        }
    }
    return true;
}

int main( int argc, char** argv )
{
    if( !consoleInput( argc, argv ) )
    {
        args::width  = 1366;
        args::height = 768;
    }

    sfml_window::window mainWindow(args::width, args::height);
    mainWindow.createWindow("Render view");

    int stepFunc = HYPERBOLIC_SIGMOID;
    int resX = 128, resY = 128, resZ = 128;
    hfrep3D::Point3D cen( resX/2.0f, resY/2.0f, resZ/2.0f );

    //setting up FRep object
    std::cout << "stage0: computing FRep:";
    hfrep3D::FRepObj3D FRep( resX, resY, resZ, 4.0f );
    //std::vector<float> frepSp = FRep.getFRep3D( cen, 40.0f, std::bind(&frep3D::FRepObj3D::sphere, FRep, _1, _2, _3));
    std::vector<float> frepH = FRep.getFRep3D( cen, std::bind(&hfrep3D::FRepObj3D::heart3D, FRep, _1, _2 ));
    std::cout << " Success" << std::endl;

    //computing HFRep object
    std::cout << "stage1: computing HFRep:";
    hfrep3D::HFRep3D HFRep( resX, resY, resZ );
    HFRep.calculateHFRep3D( &frepH, stepFunc, 0.0001f );
    std::vector<float> hfrep0 = HFRep.getHFRepObj3D();

    if( mainWindow.checkError(0) )
        std::cout << " Success" << std::endl;

    //assigning hfrep field to 3D texture
    std::cout << "stage2a: assigning HFRep to 3D texture:";
    hfrep3D::volumeOBJ obj1;
    obj1.setBoundingBox( 1.0f, 1.0f, 1.0f );
    obj1.setRayMarchingParams( -0.0001f, 30.0f, 200 );
    obj1.createVolumetricObj( hfrep0, resX, resY, resZ );

    if( mainWindow.checkError(1) )
        std::cout << " Success" << std::endl;

    //setting up skybox textures
    std::cout << "stage3: Setting up SkyBox:";
    hfrep3D::SkyBox skybox( args::skyBoxTex, 10.0f, 10.0f, 10.0f );
    GLuint skyTexID = skybox.getSkyboxTextureID();
    obj1.setSkyboxTextureID( skyTexID );

    if( mainWindow.checkError(2) )
        std::cout << " Success" << std::endl;

    //setting up light
    std::cout << "stage4: Setting up Light & material props.:";
    hfrep3D::light light;
    light.setDirectionalLight( glm::vec3(1.0, 1.0, 1.0), 0.3f, 0.5f, glm::vec3(0.0, 5.0, 10.0));

    if( mainWindow.checkError(3) )
        std::cout << " Success" << std::endl;

    //setting up camera
    std::cout << "stage5: Setting up Camera:";
    myCamera::camera newCamera( glm::vec3( 0.0f, 0.0f, 5.0f ),
                                glm::vec3( 0.0f, 1.0f, 0.0f ),
                                   -60.0f, 0.0f, 5.0f, 0.5f );

    glm::mat4 projection = glm::perspective( glm::radians(60.0f),
                                             mainWindow.getBufferWidth()/mainWindow.getBufferHeight(),
                                             0.1f, 20.0f );
    if( mainWindow.checkError(4) )
        std::cout << " Success" << std::endl;

    GLfloat deltaTime = 0.0f, lastTime  = 0.0f;

    bool app_is_running = true;
    sf::Clock clock;

    sf::Window *win = mainWindow.getWindow();
    while( app_is_running )
    {
        sf::Time t = clock.getElapsedTime();
        GLfloat now = t.asSeconds();
        deltaTime = now - lastTime;
        lastTime  = now;

        sf::Event event;
        while(win->pollEvent(event))
        {
            if( event.type == sf::Event::Closed )
            {
                win->close();
                app_is_running = false;
            }
            else if (event.type == sf::Event::Resized)
                glViewport(0, 0, event.size.width, event.size.height);
            else if (event.type == sf::Event::KeyPressed )
            {
                mainWindow.keyboardContr();
                newCamera.keyControlSFML( mainWindow.getKeys(), deltaTime );
            }
            else if (event.type == sf::Event::MouseMoved)
            {
                mainWindow.mouseContr();
                newCamera.mouseControl( mainWindow.getXchange(), mainWindow.getYchange() );
            }
        }

        //handle user events
        glm::mat4 finViewMatrix = newCamera.calculateViewMatrix();
        glm::vec3 curCamPos = newCamera.getCameraPosition();

        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        skybox.DrawSkybox( finViewMatrix, projection );

        obj1.renderVolumetricObj( finViewMatrix, projection, curCamPos);
        obj1.useDirectionalLight( &light );

        glUseProgram( 0 );
        win->display();
    }

    return 0;
}
