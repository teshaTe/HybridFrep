#include "include/light.h"

namespace hfrep3D {

void light::setDirectionalLight(glm::vec3 col, GLfloat aIntensity, GLfloat dIntensity, glm::vec3 direction)
{
    colour           = col;
    ambientIntensity = aIntensity;
    diffuseIntensity = dIntensity;
    directionDLight  = direction;
}

void light::useDirectionalLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
                                GLfloat diffuseIntensityLocation, GLfloat directionLocation)
{
    glUniform3f( ambientColourLocation, colour.x, colour.y, colour.z );
    glUniform1f( ambientIntensityLocation, ambientIntensity );

    glUniform3f( directionLocation, directionDLight.x, directionDLight.y, directionDLight.z );
    glUniform1f( diffuseIntensityLocation, diffuseIntensity );
}

void light::setPointLight(glm::vec3 col, GLfloat aIntensity, GLfloat dIntensity,
                          glm::vec3 position, GLfloat con, GLfloat lin, GLfloat exp)
{
    colour           = col;
    ambientIntensity = aIntensity;
    diffuseIntensity = dIntensity;
    positionPLight   = position;
    constant         = con;
    linear           = lin;
    exponent         = exp;
}

void light::usePointLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                          GLuint diffuseIntensityLocation, GLuint positionLocation,
                          GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation)
{
    glUniform3f( ambientColourLocation, colour.x, colour.y, colour.z );
    glUniform1f( ambientIntensityLocation, ambientIntensity );

    glUniform3f( positionLocation, positionPLight.x, positionPLight.y, positionPLight.z );
    glUniform1f( diffuseIntensityLocation, diffuseIntensity );

    glUniform1f( constantLocation, constant );
    glUniform1f( linearLocation,   linear );
    glUniform1f( exponentLocation, exponent );
}

void light::setSpotLight(glm::vec3 col, GLfloat aIntensity, GLfloat dIntensity,
                         glm::vec3 position, glm::vec3 direction,
                         GLfloat con, GLfloat lin, GLfloat exp, GLfloat edg)
{
    setPointLight( col, aIntensity, dIntensity, position, con, lin, exp );
    directionSpLight = glm::vec3( direction );

    edge     = edg;
    procEdge = cosf( glm::radians( edge ));
}

void light::useSpotLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                         GLuint diffuseIntensityLocation, GLuint positionLocation,
                         GLuint directionLocation, GLuint constantLocation,
                         GLuint linearLocation, GLuint exponentLocation, GLuint edgeLocation)
{
    glUniform3f( ambientColourLocation, colour.x, colour.y, colour.z );
    glUniform1f( ambientIntensityLocation, ambientIntensity );

    glUniform3f( positionLocation, positionSpLight.x, positionSpLight.y, positionSpLight.z );
    glUniform3f( directionLocation, directionSpLight.x, directionSpLight.y, directionSpLight.z );
    glUniform1f( diffuseIntensityLocation, diffuseIntensity );

    glUniform1f( constantLocation, constant );
    glUniform1f( linearLocation,   linear );
    glUniform1f( exponentLocation, exponent );
    glUniform1f( edgeLocation,     procEdge );
}


}//namespace myLight
