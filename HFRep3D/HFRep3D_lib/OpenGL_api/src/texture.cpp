#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

#include <SFML/Graphics/Image.hpp>

#include <iostream>

namespace hfrep3D {

texture::texture(char *fileLoc)
{
    textureID = 0;
    width     = 0;
    height    = 0;
    bitDepth  = 0;
    fileLocation = fileLoc;
}

void texture::loadTexture()
{
    sf::Image img;
    img.loadFromFile(fileLocation);
    width = img.getSize().x;
    height = img.getSize().y;
    const unsigned char *texData = img.getPixelsPtr();

    if( !texData )
    {
        printf( "Failed to find: %s\n", fileLocation );
        return ;
    }

    glGenTextures( 1, &textureID );
    glBindTexture( GL_TEXTURE_2D, textureID );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData );
    glGenerateMipmap( GL_TEXTURE_2D );

    glBindTexture( GL_TEXTURE_2D, 0 );
    //stbi_image_free( texData );
}

void texture::useTexture()
{
    //1 - texture unit = GL_TEXTURE0 or more
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, textureID );
}

void texture::clearTexture()
{
    glDeleteTextures( 1, &textureID );
    textureID     = 0;
    width         = 0;
    height        = 0;
    bitDepth      = 0;
    fileLocation  = strdup("");
}

}//namespace hfrep3D
