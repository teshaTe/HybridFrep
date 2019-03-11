#define STB_IMAGE_IMPLEMENTATION

#include <cmath>
#include <string.h>
#include <vector>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"
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

    GLFW_window::window mainWindow( args::width, args::height );
    mainWindow.initialise();

    int stepFunc = HYPERBOLIC_SIGMOID;
    int resX = 128, resY = 128, resZ = 128;
    frep3D::Point3D cen( resX/2.0f, resY/2.0f, resZ/2.0f );

    //setting up FRep object
    std::cout << "stage0: computing FRep:";
    frep3D::FRepObj3D FRep( resX, resY, resZ, 1.0f );
    std::vector<float> frepSp = FRep.getFRep3D( cen, 40.0f,
                                                std::bind(&frep3D::FRepObj3D::sphere, FRep, _1, _2, _3));
    //std::vector<float> frepH = FRep.getFRep3D( frep3D::Point3D( 80.0f, 80.0f, 80.0f ),
    //                                            std::bind(&frep3D::FRepObj3D::heart3D, FRep, _1, _2 ));
    std::cout << " Success" << std::endl;

    //computing HFRep object
    std::cout << "stage1: computing HFRep:";
    hfrep3D::HFRep3D HFRep( resX, resY, resZ );
    HFRep.calculateHFRep3D( &frepSp, stepFunc, 0.0001f );
    std::vector<float> hfrep0 = HFRep.getHFRepObj3D();

    if( mainWindow.checkError(0) )
        std::cout << " Success" << std::endl;

    //assigning hfrep field to 3D texture
    std::cout << "stage2: assigning HFRep to 3D texture:";
    hfrep3D::volumeOBJ obj1;
    obj1.setBoundingBox( 1.0f, 1.0f, 1.0f );
    obj1.setRayMarchingParams( -0.0001f, 10.0f, 200 );
    obj1.createVolumetricObj( hfrep0, resX, resY, resZ );
    if( mainWindow.checkError(1) )
        std::cout << " Success" << std::endl;

    //setting up skybox textures
    std::cout << "stage3: Setting up SkyBox:";
    hfrep3D::SkyBox skybox( args::skyBoxTex, 10.0f, 10.0f, 10.0f );
    if( mainWindow.checkError(2) )
        std::cout << " Success" << std::endl;

    //setting up light
    std::cout << "stage4: Setting up Light & material props.:";
    hfrep3D::light light;
    light.setDirectionalLight( glm::vec3(1.0, 1.0, 1.0), 0.3f, 0.5f, glm::vec3(0.0, -5.0, 2.0));
    hfrep3D::material material0;
    material0.setSimpleMaterial(4.0f, 256.0f, 0.8f);
    if( mainWindow.checkError(2) )
        std::cout << " Success" << std::endl;

    //setting up camera
    std::cout << "stage5: Setting up Camera:";
    myCamera::camera newCamera( glm::vec3( 0.0f, 0.0f, 2.0f ),
                                glm::vec3( 0.0f, 1.0f, 0.0f ),
                                   -60.0f, 0.0f, 5.0f, 0.5f );

    glm::mat4 projection = glm::perspective( glm::radians(60.0f),
                                             mainWindow.getBufferWidth()/mainWindow.getBufferHeight(),
                                             0.1f, 20.0f );
    if( mainWindow.checkError(3) )
        std::cout << " Success" << std::endl;

    GLfloat deltaTime = 0.0f, lastTime  = 0.0f;

    while( !mainWindow.getShouldClose() )
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime  = now;

        //handle user events
        glfwPollEvents();
        newCamera.keyControl( mainWindow.getKeys(), deltaTime );
        newCamera.mouseControl( mainWindow.getXchange(), mainWindow.getYchange() );
        glm::mat4 finViewMatrix = newCamera.calculateViewMatrix();

        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        skybox.DrawSkybox( finViewMatrix, projection );

        obj1.renderVolumetricObj( finViewMatrix, projection, newCamera.getCameraPosition(), &light );
        obj1.useDirectionalLight( &light );
        obj1.useMaterial( &material0 );

        glUseProgram( 0 );
        mainWindow.swapBuffers();
    }

    return 0;
}
