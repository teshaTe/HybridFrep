#include "include/volumetricObj.h"

namespace hfrep3D {

volumeOBJ::volumeOBJ()
{
    textureObjID   = 0;
    textureFieldID = 0;
}

volumeOBJ::~volumeOBJ()
{
    clearVolumetricObj();
}

void volumeOBJ::createVolumetricObj(std::vector<float> *field, int width, int height, int depth )
{
    glGenTextures( 1, &textureFieldID );
    glBindTexture( GL_TEXTURE_3D, textureFieldID );

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, width, height, depth, 0, GL_R, GL_FLOAT, field->data());
    glGenerateMipmap( GL_TEXTURE_3D );

    glBindTexture( GL_TEXTURE_3D, 0 );
}

void volumeOBJ::renderVolumetricObj()
{
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_3D, textureFieldID );
}

void volumeOBJ::clearVolumetricObj()
{
    glDeleteTextures( 1, &textureFieldID );
    textureFieldID = 0;
}

void volumeOBJ::sliceVolumetricObj()
{

}

void volumeOBJ::createSimpleMaterial(shell surf, interior vol, std::vector<float> *field, int width, int height, int depth)
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

    glGenTextures( 2, &textureObjID );
    glBindTexture( GL_TEXTURE_3D, textureObjID );

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, width, height, depth, 0, GL_RGB, GL_FLOAT, &simpleMat[0]);
    glGenerateMipmap( GL_TEXTURE_3D );

    glBindTexture( GL_TEXTURE_3D, 0 );
}

void volumeOBJ::useSimpleMaterial()
{
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_3D, textureObjID );
}

}//namespace volumetric_object
