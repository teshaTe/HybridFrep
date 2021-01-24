#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SFMLwindow.h"
#include "camera.h"
#include "volumetricObj.h"
#include "frep3D.h"
#include "microstructures.h"
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

bool consoleInput(int argc, char** argv)
{
    for (int i = 0; i < argc; i++)
    {
        const char *cur = argv[i];
        if(!strcmp(cur, "-w"))
            args::width = std::stoi(argv[i+1]);
        if(!strcmp(cur, "-h"))
            args::height = std::stoi(argv[i+1]);
        else {
            std::cout << "Warning: default values for width and height of the window will be used! " << std::endl;
            std::cout << " width [-w] = 1366, height [-h] = 768 " << std::endl;
            return false;
        }
    }
    return true;
}

void eventsControl(sfml_window::window *mainWindow, myCamera::camera *newCamera, hfrep3D::volumeOBJ *obj,
                   sf::Window *win, GLfloat deltaTime, bool app_is_running)
{
    sf::Event event;
    while(win->pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
        {
            win->close();
            app_is_running = false;
        }
        else if (event.type == sf::Event::Resized)
            glViewport(0, 0, event.size.width, event.size.height);
        else if (event.type == sf::Event::KeyPressed)
        {
            mainWindow->keyboardContr();
            newCamera->keyControlSFML(mainWindow->getKeys(), deltaTime);
            obj->setCutPlanePositionY(mainWindow->getKeys(), deltaTime);
            obj->setInitTraceDistDynamically(mainWindow->getKeys());
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                mainWindow->saveImage(win);
        }
        else if (event.type == sf::Event::MouseMoved)
        {
            mainWindow->mouseContr();
            newCamera->mouseControl(mainWindow->getXchange(), mainWindow->getYchange());
        }
    }
}

void renderingPass(sfml_window::window *mainWindow, myCamera::camera *newCamera, hfrep3D::volumeOBJ *obj,
                   hfrep3D::SkyBox *skybox, hfrep3D::light *light)
{
    sf::Clock clock;
    GLfloat deltaTime = 0.0f, lastTime  = 0.0f;
    sf::Window *win = mainWindow->getWindow();
    bool app_is_running = true;

    glm::mat4 projection = glm::perspective(glm::radians(60.0f),
                                             mainWindow->getBufferWidth()/mainWindow->getBufferHeight(),
                                             0.1f, 20.0f);
    while(app_is_running)
    {
        sf::Time t = clock.getElapsedTime();
        GLfloat now = t.asSeconds();
        deltaTime = now - lastTime;
        lastTime  = now;

        eventsControl(mainWindow, newCamera, obj, win, deltaTime, app_is_running);

        //handle user events
        glm::mat4 finViewMatrix = newCamera->calculateViewMatrix();
        glm::vec3 curCamPos  = newCamera->getCameraPosition();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skybox->DrawSkybox(finViewMatrix, projection);

        obj->renderVolumetricObj(finViewMatrix, projection, curCamPos);
        obj->useDirectionalLight(light);

        glUseProgram(0);
        win->display();
    }
}

int main(int argc, char *argv[])
{
    if(!consoleInput(argc, argv))
    {
        args::width  = 1366;
        args::height = 768;
    }

    sfml_window::window mainWindow(args::width, args::height);
    mainWindow.createWindow("Render view");

    const int resX = 386, resY = 386, resZ = 386;
    glm::f32vec3 cen(resX/2.0f, resY/2.0f, resZ/2.0f);

    std::cout << "stage: computing FRep object: ";
    hfrep3D::FRepObj3D FRep(resX, resY, resZ, 1.0f);
    std::vector<float> frep = FRep.getFRep3D(cen, 120.0f, std::bind(&hfrep3D::FRepObj3D::sphere, FRep, _1, _2, _3));

    glm::vec3 cutPos = cen+glm::vec3(0, 0, 0);
    std::vector<float> cutFrep = FRep.getShelledObj(&frep, 50, hfrep3D::cut::X, &cutPos);

    if(mainWindow.checkError(1))
        std::cout << " Success" << std::endl;

    std::cout << "stage: computing HFRep object: ";
    hfrep3D::HFRep3D HFRep(resX, resY, resZ);
    HFRep.calculateHFRep3D(&cutFrep, hfrep3D::stepFunction::HYPERBOLIC_SIGMOID, 1e-40,
                                                      hfrep3D::distMethood::VDT3D_TIES);
    std::vector<float> hfrep1 = HFRep.getHFRepObj3D();

    if(mainWindow.checkError(2))
        std::cout << " Success" << std::endl;


    std::cout << "stage: assigning HFRep to 3D texture:";
    hfrep3D::volumeOBJ obj1;
    obj1.setBoundingBox(2.0f, 2.0f, 2.0f);

    obj1.setRayMarchingParams(0.01f, -0.001, 10.0f, 1000.0f); //RAY MARCHING PARAMS
    obj1.createVolumetricObj(hfrep1, resX, resY, resZ);

    if(mainWindow.checkError(3))
        std::cout << " Success" << std::endl;


    //setting up skybox textures
    std::cout << "stage: Setting up SkyBox:";
    hfrep3D::SkyBox skybox(args::skyBoxTex, 10.0f, 10.0f, 10.0f);
    GLuint skyTexID = skybox.getSkyboxTextureID();
    obj1.setSkyboxTextureID(skyTexID);

    if(mainWindow.checkError(4))
        std::cout << " Success" << std::endl;


    //setting up light
    std::cout << "stage: Setting up Light & material props.:";
    hfrep3D::light light;
    light.setDirectionalLight(glm::vec3(1.0, 1.0, 1.0), 0.3f, 0.5f, glm::vec3(0.0, 5.0, 10.0));

    if(mainWindow.checkError(5))
        std::cout << " Success" << std::endl;


    //setting up camera
    std::cout << "stage: Setting up Camera:";
    myCamera::camera newCamera(glm::vec3(0.0f, 0.0f, 5.0f),
                               glm::vec3(0.0f, 1.0f, 0.0f),
                                 -60.0f, 0.0f, 5.0f, 0.5f);

    if(mainWindow.checkError(6))
        std::cout << " Success" << std::endl;


    renderingPass(&mainWindow, &newCamera, &obj1, &skybox, &light);
    return 0;
}
