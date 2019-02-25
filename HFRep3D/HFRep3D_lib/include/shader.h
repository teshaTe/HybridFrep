#ifndef H_SHADER_CLASS
#define H_SHADER_CLASS

#include <iostream>
#include <string.h>
#include <fstream>

#include <GL/glew.h>

#include "include/light.h"

namespace hfrep3D {

#define MAX_POINT_LIGHTS 3
#define MAX_SPOT_LIGHTS  3

class shader
{
public:
    shader();
    ~shader() { clearShader(); }

    void clearShader();

    void createFromFiles( const char* vertexLocation, const char* fragmentLocation );
    std::string ReadFile( const char* fileLocation );
    inline void createFromString( const char* vertexCode, const char* fragmentCode ) { compileShader( vertexCode, fragmentCode); }

    void Validate();

    inline void useShader() { glUseProgram( shaderID ); }

    void setDirectionalLight( light_space::light* dLight );
    void setPointLights( light_space::light* pLight, unsigned int lightCount );
    void setSpotLights( light_space::light* sLight, unsigned int lightCount );

    void setRayMarchingParams( int totalSteps, float stopThreshold, float gradientStep,
                               float clipFarPlane, float maxDist, float minDist );

    inline GLuint getProjectionLocation()       { return uniformProjection; }
    inline GLuint getModelLocation()            { return uniformModel; }
    inline GLuint getViewLocation()             { return uniformView; }
    inline GLuint getAmbientIntensityLocation() { return uniformDirectionalLight.uniformAmbientIntensity; }
    inline GLuint getAmbientColourLocation()    { return uniformDirectionalLight.uniformColour; }
    inline GLuint getDiffuseIntensityLocation() { return uniformDirectionalLight.uniformDiffuseIntensity; }
    inline GLuint getDirection()                { return uniformDirectionalLight.uniformDirection; }
    inline GLuint getEyePositionLocation()      { return uniformEyePosition; }
    inline GLuint getSpecularIntesityLocation() { return uniformSpecularIntensity; }
    inline GLuint getSpecularPowerLocation()    { return uniformSpecularPower; }

private:
    int pointLightCount, spotLightCount;

    GLuint shaderID, uniformProjection, uniformModel,
           uniformView, uniformEyePosition;
    GLuint uniformSpecularIntensity, uniformSpecularPower;

    GLint uniformNumberOfSteps;
    GLint uniformSampler3D;

    GLfloat uniformStopThres;
    GLfloat uniformGradStep;
    GLfloat uniformClipFar;
    GLfloat uniformMaxTraceDist;
    GLfloat uniformMinHitDist;

    struct {
        GLuint uniformColour;
        GLuint uniformAmbientIntensity;
        GLuint uniformDiffuseIntensity;

        GLuint uniformDirection;
    } uniformDirectionalLight;


    GLuint uniformPointLightCount;
    struct {
        GLuint uniformColour;
        GLuint uniformAmbientIntensity;
        GLuint uniformDiffuseIntensity;

        GLuint uniformPosition;
        GLuint uniformConstant;
        GLuint uniformLinear;
        GLuint uniformExponent;
    } uniformPointLight[MAX_POINT_LIGHTS];


    GLuint uniformSpotLightCount;
    struct {
        GLuint uniformColour;
        GLuint uniformAmbientIntensity;
        GLuint uniformDiffuseIntensity;

        GLuint uniformPosition;
        GLuint uniformConstant;
        GLuint uniformLinear;
        GLuint uniformExponent;

        GLuint uniformDirection;
        GLuint uniformEdge;
    } uniformSpotLight[MAX_SPOT_LIGHTS];

    void compileShader( const char* vertexCode, const char* fragmentCode );
    void addShader( GLuint TheProgram, const char* ShaderCode, GLenum ShaderType );
};

}//namespace shader_space
#endif
