#version 330

layout ( location = 0 ) in vec3 pos;

out vec3 texCoord0;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec4 position = projection * view * vec4( pos, 1.0 );
    gl_Position   = position.xyww;
    texCoord0     = pos;
}
