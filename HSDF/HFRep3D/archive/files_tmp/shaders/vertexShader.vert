#version 330

layout ( location = 0 ) in vec3 pos;
layout ( location = 1 ) in vec2 tex2D;
layout ( location = 2 ) in vec3 tex3D

out vec4 vCol;
out vec2 texCoord2D_0;
out vec3 texCoord3D_0;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * model * vec4( pos, 1.0 );
    vCol        = vec4( clamp(pos, 0.0f, 1.0f), 1.0f );

    texCoord2D_0 = tex2D;
    texCoord3D_0 = tex3D;

    FragPos     = (model * vec4( pos, 1.0 )).xyz; // assign certain values to the current variable, swizzling
}
