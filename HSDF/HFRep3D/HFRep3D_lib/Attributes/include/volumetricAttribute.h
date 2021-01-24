#ifndef H_MATERIAL_CLASS
#define H_MATERIAL_CLASS

#include <vector>

#include <GL/glew.h>

namespace material {

typedef std::pair<std::vector<float>, std::vector<float>> HObj_single_mat;

class attribute
{
public:
    attribute() {}
    ~attribute() {}

    void setSimpleVolMaterial(HObj_single_mat &field, int width, int height, int depth);
    void useSimpleVolMaterial();

    void setSimpleMaterial(GLfloat sIntensity, GLfloat sPower, GLfloat FresnelCoeff);
    void useSimpleMaterial(GLuint specularIntesityLocation, GLuint specularPowerLocation, GLuint FresnelCoeffLocation);

private:
    GLuint textureID;

    GLfloat specularIntensity; //how much the light will be absorbed by the object
    GLfloat specularPower;     //how smooth the surface will be, collected in one region
    GLfloat FresnelC;          //Fresnel Coefficient which is needed for physicaly based reflections

    std::vector<float> simpleMat;

    void checkError(int number);

};

}// namespace hfrep3D

#endif // H_MATERIAL_CLASS
