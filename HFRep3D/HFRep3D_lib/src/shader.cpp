#include "include/shader.h"

namespace hfrep3D {

shader::shader()
{
    shaderID          = 0;
    uniformModel      = 0;
    uniformProjection = 0;
    pointLightCount   = 0;
    spotLightCount    = 0;
}

void shader::createFromFiles(const char *vertexLocation, const char *fragmentLocation)
{
    std::string vertexString   = ReadFile( vertexLocation );
    std::string fragmentString = ReadFile( fragmentLocation );

    const char* vertexCode   = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();

    compileShader( vertexCode, fragmentCode );
}

std::string shader::ReadFile(const char *fileLocation)
{
    std::string content;
    std::ifstream fileStream( fileLocation, std::ios::in );
    if ( !fileStream )
    {
        printf( "Failed to read %s! File does not exist! \n", fileLocation );
        return "";
    }

    std::string line  = "";
    while ( !fileStream.eof() )
    {
        std::getline( fileStream, line );
        content.append( line + "\n" );
    }

    fileStream.close();
    return content;
}


void shader::compileShader(const char *vertexCode, const char *fragmentCode)
{
    shaderID = glCreateProgram();
    if ( !shaderID )
    {
        printf("Error: unable creating shader program! \n");
        return;
    }
    addShader( shaderID, vertexCode,   GL_VERTEX_SHADER );
    addShader( shaderID, fragmentCode, GL_FRAGMENT_SHADER );

    // elogging for shader error handling
    GLint  result = 0;
    GLchar elog[1024] = { 0 };

    glLinkProgram( shaderID );// creating executables on the GPU
    //checking program status after compiling
    glGetProgramiv( shaderID, GL_LINK_STATUS, &result );
    if( !result )
    {
        glGetProgramInfoLog( shaderID, sizeof ( elog ), NULL, elog );
        printf( "Error: linking program < %s > \n", elog );
        return;
    }

    //validating the program
    glValidateProgram( shaderID );
    glGetProgramiv( shaderID, GL_VALIDATE_STATUS, &result );
    if( !result )
    {
        glGetProgramInfoLog( shaderID, sizeof ( elog ), NULL, elog );
        printf( "Error: validating program < %s > \n", elog );
        return;
    }

    uniformProjection  = glGetUniformLocation( shaderID, "projection" );
    uniformView        = glGetUniformLocation( shaderID, "view" );
    uniformEyePosition = glGetUniformLocation( shaderID, "eyePosition" );

    glUniform1i(uniformSampler3D, 0);
    uniformSampler3D = glGetUniformLocation(shaderID, "HFRepField");

    /*uniformDirectionalLight.uniformColour  = glGetUniformLocation( shaderID, "directionalLight.base.colour" );
    uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation( shaderID, "directionalLight.base.ambientIntensity" );

    uniformDirectionalLight.uniformDirection        = glGetUniformLocation( shaderID, "directionalLight.direction" );
    uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation( shaderID, "directionalLight.base.diffuseIntensity" );
*/
    /*uniformSpecularIntensity = glGetUniformLocation( shaderID, "material0.specularIntensity" );
    uniformSpecularPower     = glGetUniformLocation( shaderID, "material0.specularPower" );

    uniformPointLightCount = glGetUniformLocation( shaderID, "pointLightCount" );
    uniformSpotLightCount  = glGetUniformLocation( shaderID, "spotLightCount" );

    for ( int i = 0; i < MAX_POINT_LIGHTS; i++ )
    {
        char locBuffer[100] = { '\0' };
        snprintf( locBuffer, sizeof( locBuffer ), "pointLights[%d].base.colour", i );
        uniformPointLight[i].uniformColour = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "pointLights[%d].base.ambientIntensity", i );
        uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "pointLights[%d].base.diffuseIntensity", i );
        uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "pointLights[%d].position", i );
        uniformPointLight[i].uniformPosition = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "pointLights[%d].constant", i );
        uniformPointLight[i].uniformConstant = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "pointLights[%d].linear", i );
        uniformPointLight[i].uniformLinear = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "pointLights[%d].exponent", i );
        uniformPointLight[i].uniformExponent = glGetUniformLocation( shaderID, locBuffer );
    }

    for ( int i = 0; i < MAX_SPOT_LIGHTS; i++ )
    {
        char locBuffer[100] = { '\0' };
        snprintf( locBuffer, sizeof( locBuffer ), "spotLights[%d].base.base.colour", i );
        uniformSpotLight[i].uniformColour = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "spotLights[%d].base.base.ambientIntensity", i );
        uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "spotLights[%d].base.base.diffuseIntensity", i );
        uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "spotLights[%d].base.position", i );
        uniformSpotLight[i].uniformPosition = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "spotLights[%d].base.constant", i );
        uniformSpotLight[i].uniformConstant = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "spotLights[%d].base.linear", i );
        uniformSpotLight[i].uniformLinear = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "spotLights[%d].base.exponent", i );
        uniformSpotLight[i].uniformExponent = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "spotLights[%d].direction", i );
        uniformSpotLight[i].uniformExponent = glGetUniformLocation( shaderID, locBuffer );

        snprintf( locBuffer, sizeof( locBuffer ), "spotLights[%d].edge", i );
        uniformSpotLight[i].uniformExponent = glGetUniformLocation( shaderID, locBuffer );
    }
*/
}

void shader::addShader(GLuint TheProgram, const char *ShaderCode, GLenum ShaderType)
{
    GLuint TheShader = glCreateShader( ShaderType ); //creating empty shader of choosen type

    const GLchar *theCode[1];
    theCode[0] = ShaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen( ShaderCode );

    glShaderSource( TheShader, 1, theCode, codeLength );
    glCompileShader( TheShader );

    // elogging for shader error handling
    GLint  result = 0;
    GLchar elog[1024] = { 0 };

    //checking program status after compiling
    glGetShaderiv( TheShader, GL_COMPILE_STATUS, &result );
    if( !result )
    {
        glGetShaderInfoLog( TheShader, sizeof ( elog ), NULL, elog );
        printf( "Error: compiling %d shader < %s > \n", ShaderType, elog );
        return;
    }

    glAttachShader( TheProgram, TheShader );
}

void shader::clearShader()
{
    if( shaderID != 0 )
    {
        glDeleteProgram ( shaderID );
        shaderID = 0;
    }

    uniformModel      = 0;
    uniformProjection = 0;
}

void shader::setDirectionalLight(light_space::light *dLight)
{
    dLight->useDirectionalLight( uniformDirectionalLight.uniformAmbientIntensity,
                                 uniformDirectionalLight.uniformColour,
                                 uniformDirectionalLight.uniformDiffuseIntensity,
                                 uniformDirectionalLight.uniformDirection );
}

void shader::setPointLights(light_space::light *pLight, unsigned int lightCount)
{
    if( lightCount > MAX_POINT_LIGHTS ) lightCount = MAX_POINT_LIGHTS;

       glUniform1i( uniformPointLightCount, lightCount );

       for ( size_t i = 0; i < lightCount; i++)
       {
           pLight[i].usePointLight( uniformPointLight[i].uniformAmbientIntensity, uniformPointLight[i].uniformColour,
                                    uniformPointLight[i].uniformDiffuseIntensity, uniformPointLight[i].uniformPosition,
                                    uniformPointLight[i].uniformConstant, uniformPointLight[i].uniformLinear,
                                    uniformPointLight[i].uniformExponent );
       }
}

void shader::setSpotLights(light_space::light *sLight, unsigned int lightCount)
{
    if( lightCount > MAX_SPOT_LIGHTS ) lightCount = MAX_SPOT_LIGHTS;

        glUniform1i( uniformSpotLightCount, lightCount );

        for ( size_t i = 0; i < lightCount; i++)
        {
            sLight[i].useSpotLight( uniformSpotLight[i].uniformAmbientIntensity, uniformSpotLight[i].uniformColour,
                                    uniformSpotLight[i].uniformDiffuseIntensity, uniformSpotLight[i].uniformPosition,
                                    uniformSpotLight[i].uniformDirection, uniformSpotLight[i].uniformConstant,
                                    uniformSpotLight[i].uniformLinear, uniformSpotLight[i].uniformExponent,
                                    uniformSpotLight[i].uniformEdge );
        }
}

void hfrep3D::shader::Validate()
{
    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        std::cerr << "Error validating program:" << eLog << std::endl;
        return;
    }
}


}//namespace shader_space
