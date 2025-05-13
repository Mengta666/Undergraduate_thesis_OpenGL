#version 430 core

layout(location = 0) out vec4 FragColor;

in vec3 v_TexCoord;

uniform samplerCube skyBoxTexture;

void main()
{
	FragColor = texture(skyBoxTexture,v_TexCoord);
}