#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;


out vec2 v_TexCoord;
out vec3 Normal;
out vec3 FragPos; 
out vec4 DirectionalLightSpacePos;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;						// project *  view

void main()
{
	DirectionalLightSpacePos = lightSpaceMatrix * model * vec4(position,1.0f);

	v_TexCoord = texCoord;
	
	/* 重设法线，让纹理看起来更加真实 */
	mat3 NormalMatrix = mat3(transpose(inverse(model)));										
	Normal = mat3(transpose(inverse(model))) * normals;                                                 // 确保法向量始终垂直与物体表面(****)
	vec3 T = normalize(NormalMatrix * tangent);
    vec3 N = normalize(NormalMatrix * normals);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    TBN = transpose(mat3(T, B, N)); 

	FragPos = vec3( model * vec4(position, 1.0) );
	gl_Position = projection * view * model * vec4(position, 1.0);
//	gl_Position.x += offSetX;
}


