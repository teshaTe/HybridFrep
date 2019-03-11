#include "include/material.h"

#include <iostream>

namespace hfrep3D {

void material::setSimpleVolMaterial(std::vector<float> *field, shell surf, interior vol, int width, int height, int depth)
{
    simpleMat.resize( field->size()*3 );
    int index = 0;
    for (size_t i = 0; i < field->size(); i++)
    {
        if( field->at(i) >= 0.0f && field->at(i) < surf.thickness )
        {
            simpleMat[index]   = surf.r_shell;
            simpleMat[index+1] = surf.g_shell;
            simpleMat[index+2] = surf.b_shell;
            index += 3;
        }
        else if( field->at(i) < 0.0f )
        {
            simpleMat[index]   = 0.0;
            simpleMat[index+1] = 0.0;
            simpleMat[index+2] = 0.0;
            index += 3;
        }
        else {
            simpleMat[index]   = vol.r_inside;
            simpleMat[index+1] = vol.g_inside;
            simpleMat[index+2] = vol.b_inside;
            index += 3;
        }
    }

    glGenTextures( 2, &textureID );
    glBindTexture( GL_TEXTURE_3D, textureID );

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, width, height, depth, 0, GL_RGB, GL_FLOAT, &simpleMat[0]);
    glGenerateMipmap( GL_TEXTURE_3D );

    checkError(0);

    glBindTexture( GL_TEXTURE_3D, 0 );
}

void material::useSimpleVolMaterial()
{
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_3D, textureID );
}

void material::setSimpleMaterial(GLfloat sIntensity, GLfloat sPower, GLfloat FresnelCoeff)
{
    specularIntensity = sIntensity;
    specularPower     = sPower;
    FresnelC          = FresnelCoeff;
}

void material::useSimpleMaterial(GLuint specularIntesityLocation, GLuint specularPowerLocation, GLuint FresnelCoeffLocation)
{
    glUniform1f( specularIntesityLocation, specularIntensity );
    glUniform1f( specularPowerLocation, specularPower );
    glUniform1f( FresnelCoeffLocation, FresnelC );
}

void material::checkError(int number)
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

}//namespace hfrep3D
