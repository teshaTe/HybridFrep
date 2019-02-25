#ifndef H_LIGHT_CLASS
#define H_LIGHT_CLASS

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace light_space {

class light
{
public:
    light() {}
    ~light() {}

    void setDirectionalLight( GLfloat red, GLfloat green, GLfloat blue,
                              GLfloat aIntensity, GLfloat dIntensity,
                              GLfloat xDir, GLfloat yDir, GLfloat zDir );
    void useDirectionalLight( GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
                              GLfloat diffuseIntensityLocation, GLfloat directionLocation);

    void setPointLight( GLfloat red, GLfloat green, GLfloat blue,
                        GLfloat aIntensity, GLfloat dIntensity,
                        GLfloat xPos, GLfloat yPos, GLfloat zPos,
                        GLfloat con, GLfloat lin, GLfloat exp);
    void usePointLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                       GLuint diffuseIntensityLocation, GLuint positionLocation,
                       GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation);

    void setSpotLight(GLfloat red, GLfloat green, GLfloat blue,
                      GLfloat aIntensity, GLfloat dIntensity,
                      GLfloat xPos, GLfloat yPos, GLfloat zPos,
                      GLfloat xDir, GLfloat yDir, GLfloat zDir,
                      GLfloat con, GLfloat lin, GLfloat exp, GLfloat edg);
    void useSpotLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                      GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionLocation,
                      GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
                      GLuint edgeLocation);

private:
    glm::vec3 colour;
    glm::vec3 directionDLight, directionSpLight;
    glm::vec3 positionPLight,  positionSpLight;

    GLfloat ambientIntensity;
    GLfloat diffuseIntensity;

    GLfloat constant, linear, exponent;
    GLfloat edge, procEdge;
};

}//namespace myLight
#endif
