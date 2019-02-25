#define STB_IMAGE_IMPLEMENTATION

#include "include/skybox.h"
#include "include/stb_image.h"

namespace hfrep3D {

SkyBox::SkyBox(std::vector<std::string> faceLocations)
{
    skyShader = std::make_shared<shader>();
    std::string fShader = "shaders/FragSkyBoxShader.frag";
    std::string vShader = "shaders/VertSkyBoxShader.vert";
    skyShader.get()->createFromFiles( vShader.c_str(), fShader.c_str() );

    uniformProjection = skyShader.get()->getProjectionLocation();
    uniformView       = skyShader.get()->getViewLocation();

    //Texture Setup
    glGenTextures( 1, &textureID );
    glBindTexture( GL_TEXTURE_CUBE_MAP, textureID );

    int width, height, bitDepth;

    for (int i = 0; i < 6; i++)
    {
        unsigned char *texData = stbi_load( faceLocations[i].c_str(), &width, &height, &bitDepth, 0 );
        if( !texData )
        {
            std::cerr << "ERROR [skybox]: failed to find: " << faceLocations[i] << std::endl;
            stbi_image_free( texData );
            return;
        }

        glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData );
        stbi_image_free( texData );
    }

    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    //set up SkyBox Mesh
    unsigned int skyboxIndices[] = {
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

    float skyboxVertices[] = {
            -200.0f,  200.0f, -200.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
            -200.0f, -200.0f, -200.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
             200.0f,  200.0f, -200.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
             200.0f, -200.0f, -200.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,

            -200.0f,  200.0f, 200.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
             200.0f,  200.0f, 200.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
            -200.0f, -200.0f, 200.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
             200.0f, -200.0f, 200.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f
        };

    skyMesh = std::make_shared<mesh>();
    skyMesh.get()->CreateMesh( skyboxVertices, skyboxIndices, 64, 36 );
}

void SkyBox::DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
    viewMatrix = glm::mat4( glm::mat3( viewMatrix ) );
    glDepthMask( GL_FALSE );

    skyShader.get()->useShader();
    glUniformMatrix4fv( uniformProjection, 1, GL_FALSE, glm::value_ptr( projectionMatrix ) );
    glUniformMatrix4fv( uniformView, 1, GL_FALSE, glm::value_ptr( viewMatrix ) );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, textureID );

    skyShader.get()->Validate();
    skyMesh.get()->RenderMesh();

    glDepthMask( GL_TRUE );
}

}// namespace hfrep3D
