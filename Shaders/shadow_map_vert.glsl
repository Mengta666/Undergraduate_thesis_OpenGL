#version 430 core

layout (location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;


uniform mat4 model;
uniform mat4 lightSpaceMatrix;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

uniform bool AnimationShadow;

void main()
{
	if(AnimationShadow)
	{
	    vec4 totalPosition = vec4(0.0f);
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(boneIds[i] == -1) 
                continue;
            if(boneIds[i] >=MAX_BONES) 
            {
                totalPosition = vec4(pos,1.0f);
                break;
            }
            vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);
            totalPosition += localPosition * weights[i];
        }
        gl_Position =  lightSpaceMatrix * model * totalPosition;
	}

	else{
		gl_Position = lightSpaceMatrix * model * vec4(pos, 1.0);
	}


}
