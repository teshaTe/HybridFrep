#ifndef H_SHADER_CLASS
#define H_SHADER_CLASS

#include <iostream>
#include <string.h>
#include <fstream>

#include <GL/glew.h>

#include "light.h"

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

    void setDirectionalLight( light* dLight );
    void setPointLights( light* pLight, unsigned int lightCount );
    void setSpotLights( light* sLight, unsigned int lightCount );

    void setRayMarchingParams( float startTraceDist, float minHitDist, float maxTraceDist, int stepNumber );
    void setBoundingBoxSize(glm::vec3 minSize, glm::vec3 maxSize );

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
    inline GLuint getHFRep3DLocation()          { return uniformHFRep3D; }
    inline GLuint getSkyboxSamplerLocation()    { return uniformSkyboxSampler; }
    inline GLuint getCutPlanePosYLocation()     { return uniformCutPlanePosY; }
    inline GLuint getIsoLineModeLocation()      { return uniformIsoLineMode; }

private:
    int pointLightCount, spotLightCount;

    //setting basic parameters for holding the shader
    GLuint shaderID, uniformProjection, uniformModel, uniformView, uniformIsoLineMode,
           uniformEyePosition, uniformHFRep3D, uniformSkyboxSampler, uniformCutPlanePosY;

    //setting basic params for holding the light simple based on Phong Model
    GLuint uniformSpecularIntensity, uniformSpecularPower, uniformFresnelCoeff;

    //setting up parameters for holding sphere-tracing
    GLuint uniformGradStep, uniformMinHitDist, uniformStartTraceDist,
           uniformMaxTraceDist, uniformStepNumber;

    //setting up bounding box parameters
    GLuint uniformAABBmin, uniformAABBmax;

    //*****************Setting up addiitonal light params (point/spot lights)*****************
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

    //****************************************************************************

    void compileShader( const char* vertexCode, const char* fragmentCode );
    void addShader( GLuint TheProgram, const char* ShaderCode, GLenum ShaderType );
};

}//namespace shader_space
#endif
