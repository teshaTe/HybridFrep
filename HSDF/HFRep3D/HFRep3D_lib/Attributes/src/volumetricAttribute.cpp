#include "volumetricAttribute.h"

#include <iostream>

namespace material {

void attribute::setSimpleVolMaterial(HObj_single_mat &field, int width, int height, int depth)
{

    glGenTextures(2, &textureID);
    glBindTexture(GL_TEXTURE_3D, textureID);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, width, height, depth, 0, GL_RGB, GL_FLOAT, &simpleMat[0]);
    glGenerateMipmap(GL_TEXTURE_3D);

    checkError(0);

    glBindTexture(GL_TEXTURE_3D, 0);
}

void attribute::useSimpleVolMaterial()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, textureID);
}

void attribute::setSimpleMaterial(GLfloat sIntensity, GLfloat sPower, GLfloat FresnelCoeff)
{
    specularIntensity = sIntensity;
    specularPower     = sPower;
    FresnelC          = FresnelCoeff;
}

void attribute::useSimpleMaterial(GLuint specularIntesityLocation, GLuint specularPowerLocation, GLuint FresnelCoeffLocation)
{
    glUniform1f(specularIntesityLocation, specularIntensity);
    glUniform1f(specularPowerLocation, specularPower);
    glUniform1f(FresnelCoeffLocation, FresnelC);
}

void attribute::checkError(int number)
{
    GLenum errCode = glGetError();
    const GLubyte *errString;
    if((errCode = glGetError()) != GL_NO_ERROR)
    {
        errString = gluErrorString(errCode);
        std::cerr << "\nERROR (while creating material) < "<< number << " > :" << errString << std::endl;
        exit(-1);
    }
}

}//namespace material
