#include "include/volumetricObj.h"

#include <string>
#include <iostream>
#include <algorithm>

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

namespace hfrep3D {

volumeOBJ::volumeOBJ()
{
    textureFieldID = 0;
    volMesh   = std::make_shared<mesh>();
    volShader = std::make_shared<shader>();
}

volumeOBJ::~volumeOBJ()
{
    clearVolumetricObj();
}

void volumeOBJ::checkError(int number)
{
    GLenum errCode = glGetError();
    const GLubyte *errString;
    if((errCode = glGetError()) != GL_NO_ERROR)
    {
        errString = gluErrorString(errCode);
        std::cerr << "\nERROR (while creating vol. obj.) < "<< number << " > :" << errString << std::endl;
        exit(-1);
    }
}

void volumeOBJ::createVolumetricObj( std::vector<float> field, float width, float height, float depth )
{
    std::string fShader = "shaders/FragHFRepShader.frag";
    std::string vShader = "shaders/VertHFRepShader.vert";
    volShader.get()->createFromFiles( vShader.c_str(), fShader.c_str() );

    uniformModel       = volShader.get()->getModelLocation();
    uniformView        = volShader.get()->getViewLocation();
    uniformProjection  = volShader.get()->getProjectionLocation();
    uniformEyePosition = volShader.get()->getEyePositionLocation();
    uniformSampler3D   = volShader.get()->getSampler3DLocation();
    uniformHFRepMinMax = volShader.get()->getHFRepMinMaxLocation();

    //auto [min, max] = std::minmax_element(std::begin(field), std::end(field));
    //HFRepMin = *min;
    //HFRepMax = *max;

    glGenTextures( 1, &textureFieldID );
    glBindTexture( GL_TEXTURE_3D, textureFieldID );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL,  4);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, width, height, depth, 0, GL_RED, GL_FLOAT, &field.front() );
    checkError(0);

    glGenerateMipmap( GL_TEXTURE_3D );
    glBindTexture( GL_TEXTURE_3D, 0 );

    //set up Mesh
    unsigned int volumeIndices[] = {
            // front
            0, 1, 2,
            2, 1, 3,
            // right
            2, 3, 5,
            5, 3, 7,
            // back
            5, 7, 4,
            4, 7, 6,
            // left
            4, 6, 0,
            0, 6, 1,
            // top
            4, 0, 5,
            5, 0, 2,
            // bottom
            1, 6, 3,
            3, 6, 7
        };

    //float x = width, y = height, z = depth;
    float volumeVertices[] = {
            -aabb_w,  aabb_h, -aabb_d,   0.0f, 0.0f, 0.0f,
            -aabb_w, -aabb_h, -aabb_d,   0.0f, 0.0f, 0.0f,
             aabb_w,  aabb_h, -aabb_d,   0.0f, 0.0f, 0.0f,
             aabb_w, -aabb_h, -aabb_d,   0.0f, 0.0f, 0.0f,

            -aabb_w,  aabb_h,  aabb_d,   0.0f, 0.0f, 0.0f,
             aabb_w,  aabb_h,  aabb_d,   0.0f, 0.0f, 0.0f,
            -aabb_w, -aabb_h,  aabb_d,   0.0f, 0.0f, 0.0f,
             aabb_w, -aabb_h,  aabb_d,   0.0f, 0.0f, 0.0f
          };

    volMesh = std::make_shared<mesh>();
    volMesh.get()->CreateMesh( volumeVertices, volumeIndices, 48, 36 );
}

void volumeOBJ::renderVolumetricObj(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 cameraPos )
{
    glm::mat4 model = glm::mat4(1);

    volShader.get()->useShader();
    glUniform1i( uniformSampler3D, 0 );
    //glUniform2f( uniformHFRepMinMax, HFRepMin, HFRepMax );
    glUniform3f( uniformEyePosition, cameraPos.x, cameraPos.y, cameraPos.z );
    glUniformMatrix4fv( uniformProjection, 1, GL_FALSE, glm::value_ptr( projectionMatrix ) );
    glUniformMatrix4fv( uniformView,       1, GL_FALSE, glm::value_ptr( viewMatrix ) );
    glUniformMatrix4fv( uniformModel,      1, GL_FALSE, glm::value_ptr( model ) );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_3D, textureFieldID );

    volShader.get()->setRayMarchingParams( minHitD, maxTraceD, stepNum );
    volShader.get()->setBoundingBoxSize( glm::vec3( -aabb_w, -aabb_h, -aabb_d ), glm::vec3( aabb_w, aabb_h, aabb_d ) );

    volShader->Validate();
    volMesh.get()->RenderMesh();
}

void volumeOBJ::clearVolumetricObj()
{
    glDeleteTextures( 1, &textureFieldID );
    textureFieldID = 0;
}

void volumeOBJ::genGradientNormals(std::vector<float> *field, float gradSt, int width, int height, int depth)
{


}

void volumeOBJ::sliceVolumetricObj()
{

}

}//namespace volumetric_object
