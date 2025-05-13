#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;


uniform vec3 particlePosition;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	float scale = 0.05f;
    gl_Position = projection * view * model * vec4((position.xyz * scale) + particlePosition,1.0);
//	gl_Position.x += offSetX;
}


