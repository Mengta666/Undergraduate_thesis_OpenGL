#version 430 core

layout (location = 0) in vec3 position;

out vec3 v_TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	v_TexCoord = position;
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
}


