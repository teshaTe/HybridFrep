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
#include "microstructures.h"
#include "skybox.h"
#include "hfrep3D.h"
#include "timer.hpp"

#include "diplib.h"
#include "diplib/multithreading.h"
#include "diplib/distance.h"

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

std::vector<float> sdfSphere(float r, glm::vec3 cent, glm::vec3 res)
{
    std::vector<float> sdf;
    for(int z = 0; z < res.z; z++)
        for(int y = 0; y < res.y; y++)
            for(int x = 0; x < res.x; x++)
            {
                float shx = std::pow(static_cast<float>(x)/res.x - cent.x/res.x, 2.0);
                float shy = std::pow(static_cast<float>(y)/res.y - cent.y/res.y, 2.0);
                float shz = std::pow(static_cast<float>(z)/res.z - cent.z/res.z, 2.0);
                float dist = r/res.x - std::sqrt(shx+shy+shz);
                sdf.push_back(dist);
            }
    return sdf;
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

int main(int argc, char** argv)
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

    prof::timer Timer;
    Timer.Start();
    //setting up FRep object
    hfrep3D::FRepObj3D FRep(resX, resY, resZ, 3.0f);
    //std::vector<float> frep = FRep.getFRep3D(cen, 120.0f, std::bind(&hfrep3D::FRepObj3D::sphere, FRep, _1, _2, _3));
    //std::vector<float> frep = FRep.getFRep3D(cen, 80, 40, std::bind(&hfrep3D::FRepObj3D::coneX, FRep, _1, _2, _3, _4));  //NOT RENDERING PROPERLY
    std::vector<float> frep = FRep.getFRep3D(cen, std::bind(&hfrep3D::FRepObj3D::heart3D, FRep, _1, _2));
    //std::vector<float> frep = FRep.getFRep3D(cen, 15.0f, 30.0f, std::bind(&hfrep3D::FRepObj3D::cylinderX, FRep, _1, _2, _3, _4));
    //std::vector<float> frep = FRep.getFRep3D(cen, 120.0f, 40.0f, std::bind(&hfrep3D::FRepObj3D::torus, FRep, _1, _2, _3, _4));
    //std::vector<float> frep = FRep.getFRep3D(cen, 1.2, 0.2, std::bind(&hfrep3D::FRepObj3D::ellipticCylinderX, FRep, _1, _2, _3, _4)); //NOT RENDERING PROPERLY
    //std::vector<float> frep = FRep.getFRep3D(cen, 30, 30, 100, std::bind(&hfrep3D::FRepObj3D::box, FRep, _1, _2, _3, _4, _5));
    //std::vector<float> frep = FRep.getFRep3D(cen, 5, 2, 5, std::bind(&hfrep3D::FRepObj3D::ellipsoid, FRep, _1, _2, _3, _4, _5));
    //std::vector<float> frep = FRep.getFRep3D(cen, 5, 2, 5, 3, 3, std::bind(&hfrep3D::FRepObj3D::superEllipsoid, FRep, _1, _2, _3, _4, _5, _6, _7));
    //std::vector<float> frep = FRep.getFRep3D(cen, 100, 50, 100, std::bind(&hfrep3D::FRepObj3D::truncPyramid, FRep, _1, _2, _3, _4, _5));
    //std::vector<float> frep   = FRep.getFRep3D(cen, std::bind(&hfrep3D::FRepObj3D::flower, FRep, _1, _2));

    /*
     * problems: cylinder, elliptic cylinder, cone, box, superEllipsod (add scaling)
     */

    //std::vector<float> sdf = sdfSphere(70.f, cen, glm::vec3(resX, resY, resZ));

    hfrep3D::microStruct micStr(resX, resY, resZ);
    std::vector<float> micStr0 = micStr.computeInfSquareSlabsX(glm::vec3(0.7,0.7,0.7), glm::vec3(0.3,0.3,0.3), glm::vec3(0.5,0.5,0.5));
    std::vector<float> micStr1 = micStr.computeInfSquareSlabsZ(glm::vec3(0.7,0.7,0.7), glm::vec3(0.3,0.3,0.3), glm::vec3(0.5,0.5,0.5));
    std::vector<float> micStr2 = micStr.computeInfSquareSlabsY(glm::vec3(0.7,0.7,0.7), glm::vec3(0.3,0.3,0.3), glm::vec3(0.5,0.5,0.5));
    //std::vector<float> micStr1 = micStr.computeInfCircleEllipseSlabsZ(glm::vec3(0.7,0.7,0.7), glm::vec3(0.3,0.3,0.3), glm::vec3(0.5,0.5,0.5));
    //std::vector<float> micStr2 = micStr.computeInfCircleEllipseSlabsY(glm::vec3(0.7,0.7,0.7), glm::vec3(0.3,0.3,0.3), glm::vec3(0.5,0.5,0.5));
    std::vector<float> micGrid0 = FRep.getFRep3D(micStr0, micStr1, 0, 0, std::bind(&hfrep3D::FRepObj3D::union_function, FRep, _1, _2, _3, _4));
    std::vector<float> micGrid = FRep.getFRep3D(micGrid0, micStr2, 0, 0, std::bind(&hfrep3D::FRepObj3D::union_function, FRep, _1, _2, _3, _4));
    glm::vec3 cutPos = cen+glm::vec3(0, 0, 0);
    std::vector<float> frepStr1 = micStr.calcObjWithMStruct(&frep, &micGrid, 50.0f, hfrep3D::frepCut::Z, &cutPos);

    Timer.End("FRep generation: ");
    //std::vector<float> frepStr1 = micStr.computeSphereCellStructure(glm::vec3(0,0,0),
    //                                                                glm::vec3(resX, resY, resZ),
    //                                                                10.0f, hfrep3D::replicFunction::TriangleWave1);*/
    //computing HFRep object
    hfrep3D::HFRep3D HFRep(resX, resY, resZ);
    //glm::vec2 surfLim = FRep.findZeroLevelSetInterval(frep, 3500); //350 - 64^3; 35 - 32^3
    //HFRep.calculateHFRep3D(&frep, hfrep3D::stepFunction::HYPERBOLIC_SIGMOID, 0.000001f, hfrep3D::distMethood::FIM3D, &surfLim);
    //std::vector<float> hfrep1 = HFRep.getHFRepObj3D();
    //std::vector<float> udf0 = HFRep.getDDT3D();

    HFRep.calculateHFRep3D(&frepStr1, hfrep3D::stepFunction::HYPERBOLIC_SIGMOID, 1e-40, hfrep3D::distMethood::VDT3D_TIES);
    std::vector<float> hfrep1 = HFRep.getHFRepObj3D();
    //std::vector<float> udf0 = HFRep.getDDT3D();

    float averErrFIM = 0, averErrDDT1 = 0, averErrDDT2=0, averErrFIMhfrep = 0, averErrDDThfrep = 0;
/*    for(size_t i = 0; i < udf1.size(); i++)
    {
        //averErrFIM  += std::abs(udf0[i] - std::abs(sdf[i]))/static_cast<float>(udf0.size());
        //averErrDDT1 += std::abs(std::abs(udf1[i]) - std::abs(udf2[i]))/static_cast<float>(udf1.size());
        averErrDDT2 += std::abs(std::abs(udf1[i]) - std::abs(sdf[i]))/static_cast<float>(udf1.size());

        //averErrFIMhfrep += std::abs(std::abs(hfrep0[i]) - std::abs(sdf[i]))/static_cast<float>(udf1.size());
        averErrDDThfrep += std::abs(std::abs(hfrep1[i]) - std::abs(sdf[i]))/static_cast<float>(udf1.size());
    }

    std::cout << std::endl;
    //std::cout << "\nFIM3D, averErr = " << averErrFIM << std::endl;
    //std::cout << "\nDDT3D1, averErr = " << averErrDDT1 << std::endl;
    std::cout << "\nDDT3D2, averErr = " << averErrDDT2 << std::endl;
    //std::cout << std::endl;
    //std::cout << "FIM3D, averErrHFrep = " << averErrFIMhfrep << std::endl;
    std::cout << "DDT3D, averErrHFRep = " << averErrDDThfrep << std::endl;
    //std::cout << std::endl;
*/

    //std::vector<float> hfrep1 = sdfSphere(20, cen, glm::vec3(resX, resY, resZ));
            //sdfSphere(20, cen, glm::vec3(resX, resY, resZ));//sdfPrism(cen, glm::vec3(resX,resY,resZ), glm::vec2(30, 40));

    //assigning hfrep field to 3D texture
    std::cout << "stage: assigning HFRep to 3D texture:";
    hfrep3D::volumeOBJ obj1;
    obj1.setBoundingBox(2.0f, 2.0f, 2.0f);

    obj1.setRayMarchingParams(0.01f, -0.001, 10.0f, 1000.0f); //RAY MARCHING PARAMS
    obj1.createVolumetricObj(hfrep1, resX, resY, resZ);

    if(mainWindow.checkError(1))
        std::cout << " Success" << std::endl;

    //setting up skybox textures
    std::cout << "stage: Setting up SkyBox:";
    hfrep3D::SkyBox skybox(args::skyBoxTex, 10.0f, 10.0f, 10.0f);
    GLuint skyTexID = skybox.getSkyboxTextureID();
    obj1.setSkyboxTextureID(skyTexID);

    if(mainWindow.checkError(2))
        std::cout << " Success" << std::endl;

    //setting up light
    std::cout << "stage: Setting up Light & material props.:";
    hfrep3D::light light;
    light.setDirectionalLight(glm::vec3(1.0, 1.0, 1.0), 0.3f, 0.5f, glm::vec3(0.0, 5.0, 10.0));

    if(mainWindow.checkError(3))
        std::cout << " Success" << std::endl;

    //setting up camera
    std::cout << "stage: Setting up Camera:";
    myCamera::camera newCamera(glm::vec3(0.0f, 0.0f, 5.0f),
                                glm::vec3(0.0f, 1.0f, 0.0f),
                                   -60.0f, 0.0f, 5.0f, 0.5f);

    glm::mat4 projection = glm::perspective(glm::radians(60.0f),
                                             mainWindow.getBufferWidth()/mainWindow.getBufferHeight(),
                                             0.1f, 20.0f);
    if(mainWindow.checkError(4))
        std::cout << " Success" << std::endl;

    GLfloat deltaTime = 0.0f, lastTime  = 0.0f;

    bool app_is_running = true;
    sf::Clock clock;

    sf::Window *win = mainWindow.getWindow();
    while(app_is_running)
    {
        sf::Time t = clock.getElapsedTime();
        GLfloat now = t.asSeconds();
        deltaTime = now - lastTime;
        lastTime  = now;

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
                mainWindow.keyboardContr();
                newCamera.keyControlSFML(mainWindow.getKeys(), deltaTime);
                obj1.setCutPlanePositionY(mainWindow.getKeys(), deltaTime);
                obj1.setInitTraceDistDynamically(mainWindow.getKeys());
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                    mainWindow.saveImage(win);
            }
            else if (event.type == sf::Event::MouseMoved)
            {
                mainWindow.mouseContr();
                newCamera.mouseControl(mainWindow.getXchange(), mainWindow.getYchange());
            }
        }

        //handle user events
        glm::mat4 finViewMatrix = newCamera.calculateViewMatrix();
        glm::vec3 curCamPos  = newCamera.getCameraPosition();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skybox.DrawSkybox(finViewMatrix, projection);

        obj1.renderVolumetricObj(finViewMatrix, projection, curCamPos);
        obj1.useDirectionalLight(&light);

        glUseProgram(0);
        win->display();
    }

    return 0;
}
