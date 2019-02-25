#include "include/light.h"

namespace light_space {

void light::setDirectionalLight(GLfloat red, GLfloat green, GLfloat blue,
                                GLfloat aIntensity, GLfloat dIntensity,
                                GLfloat xDir, GLfloat yDir, GLfloat zDir)
{
    colour = glm::vec3( red, green, blue );
    ambientIntensity = aIntensity;
    diffuseIntensity = dIntensity;
    directionDLight = glm::vec3( xDir, yDir, zDir );
}

void light::useDirectionalLight(GLfloat ambientIntensityLocation, GLfloat ambientColourLocation,
                                GLfloat diffuseIntensityLocation, GLfloat directionLocation)
{
    glUniform3f( ambientColourLocation, colour.x, colour.y, colour.z );
    glUniform1f( ambientIntensityLocation, ambientIntensity );

    glUniform3f( directionLocation, directionDLight.x, directionDLight.y, directionDLight.z );
    glUniform1f( diffuseIntensityLocation, diffuseIntensity );
}

void light::setPointLight(GLfloat red, GLfloat green, GLfloat blue,
                          GLfloat aIntensity, GLfloat dIntensity,
                          GLfloat xPos, GLfloat yPos, GLfloat zPos,
                          GLfloat con, GLfloat lin, GLfloat exp)
{
    colour = glm::vec3( red, green, blue );
    ambientIntensity = aIntensity;
    diffuseIntensity = dIntensity;
    positionPLight = glm::vec3( xPos, yPos, zPos );
    constant = con;
    linear   = lin;
    exponent = exp;
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

void light::setSpotLight(GLfloat red, GLfloat green, GLfloat blue,
                         GLfloat aIntensity, GLfloat dIntensity,
                         GLfloat xPos, GLfloat yPos, GLfloat zPos,
                         GLfloat xDir, GLfloat yDir, GLfloat zDir,
                         GLfloat con, GLfloat lin, GLfloat exp, GLfloat edg)
{
    setPointLight( red, green, blue, aIntensity, dIntensity, xPos, yPos, zPos, con, lin, exp );
    directionSpLight = glm::vec3( xDir, yDir, zDir );

    edge = edg;
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
