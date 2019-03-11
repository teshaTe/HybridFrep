#version 330 core

layout ( location = 0 ) in vec3 pos;

out vec3 FragPos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    FragPos     = (model * vec4( pos, 1.0 )).xyz;
    gl_Position = projection * view * model * vec4( pos, 1.0 );
}
