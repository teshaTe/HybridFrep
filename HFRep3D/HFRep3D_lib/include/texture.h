#ifndef H_TEXTURE_CLASS
#define H_TEXTURE_CLASS

#include <vector>
#include <string>
#include <string.h>
#include <GL/glew.h>

#include "stb_image.h"

namespace hfrep3D {

class texture
{
public:
    texture( char* fileLoc );
    ~texture() { clearTexture(); }

    void loadTexture();
    void useTexture();
    void clearTexture();

private:
    GLuint textureID;
    int width, height, bitDepth;

    char *fileLocation;
};

}// namespace hfrep3D
#endif
