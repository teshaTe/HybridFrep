#version 330 core
out vec4 colour;

in vec3 texCoord0;

uniform samplerCube skybox;

void main()
{
    colour = texture(skybox, texCoord0);
}
