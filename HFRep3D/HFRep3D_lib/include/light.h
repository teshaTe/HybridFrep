#ifndef H_LIGHT_CLASS
#define H_LIGHT_CLASS

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace hfrep3D {

class light
{
public:
    light() {}
    ~light() {}

    void setDirectionalLight(glm::vec3 col, GLfloat aIntensity, GLfloat dIntensity, glm::vec3 Direction);
    void useDirectionalLight( GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
                              GLfloat diffuseIntensityLocation, GLfloat directionLocation);

    void setPointLight(glm::vec3 col,
                        GLfloat aIntensity, GLfloat dIntensity, glm::vec3 position,
                        GLfloat con, GLfloat lin, GLfloat exp);
    void usePointLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                       GLuint diffuseIntensityLocation, GLuint positionLocation,
                       GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation);

    void setSpotLight(glm::vec3 col,
                      GLfloat aIntensity, GLfloat dIntensity, glm::vec3 position, glm::vec3 direction,
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
