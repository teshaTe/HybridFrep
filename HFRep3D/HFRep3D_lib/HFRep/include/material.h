#ifndef H_MATERIAL_CLASS
#define H_MATERIAL_CLASS

#include <vector>

#include <GL/glew.h>

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

class material
{
public:
    material() {}
    ~material() {}

    void setSimpleVolMaterial( std::vector<float> *field , shell surf, interior vol, int width, int height, int depth );
    void useSimpleVolMaterial();

    void setSimpleMaterial( GLfloat sIntensity, GLfloat sPower, GLfloat FresnelCoeff );
    void useSimpleMaterial( GLuint specularIntesityLocation, GLuint specularPowerLocation, GLuint FresnelCoeffLocation );

private:
    GLuint textureID;

    GLfloat specularIntensity; //how much the light will be absorbed bythe object
    GLfloat specularPower;     //how smooth the surface will be, collected in one region
    GLfloat FresnelC;          //Fresnel Coefficient which is needed for physicaly based reflections

    std::vector<float> simpleMat;

    void checkError(int number);

};

}// namespace hfrep3D

#endif // H_MATERIAL_CLASS
