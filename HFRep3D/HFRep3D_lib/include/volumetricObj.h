#ifndef H_VOLUMETRIC_OBJECT_CLASS
#define H_VOLUMETRIC_OBJECT_CLASS

#include <vector>
#include <string>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace hfrep3D {

struct interior
{
    float r_inside;
    float g_inside;
    float b_inside;
    GLfloat specularIntensity_inside;
    GLfloat specularPower_inside;
};

struct shell
{
    float r_shell;
    float g_shell;
    float b_shell;
    GLfloat specularIntensity_shell;
    GLfloat specularPower_shell;
    float thickness;
};

class volumeOBJ
{
public:
    volumeOBJ();
    ~volumeOBJ();

    void createVolumetricObj(std::vector<float> *field, int width, int height, int depth );
    void renderVolumetricObj();
    void clearVolumetricObj();

    void sliceVolumetricObj();

    void createSimpleMaterial(shell surf, interior vol, std::vector<float> *field , int width, int height, int depth);
    void useSimpleMaterial();

private:
    GLuint textureFieldID, textureObjID;
    GLuint uniformProjection, uniformView;

    std::vector<float> simpleMat;
};

}//namespace volumetric_object
#endif
