#pragma once

#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>


class AssimpGLMHelpers
{
public:

	static glm::mat4 ConvertMatrixToGLMMatrix(const aiMatrix4x4& from)
	{
		glm::mat4 translationMatrix;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		translationMatrix[0][0] = from.a1; translationMatrix[1][0] = from.a2; translationMatrix[2][0] = from.a3; translationMatrix[3][0] = from.a4;
		translationMatrix[0][1] = from.b1; translationMatrix[1][1] = from.b2; translationMatrix[2][1] = from.b3; translationMatrix[3][1] = from.b4;
		translationMatrix[0][2] = from.c1; translationMatrix[1][2] = from.c2; translationMatrix[2][2] = from.c3; translationMatrix[3][2] = from.c4;
		translationMatrix[0][3] = from.d1; translationMatrix[1][3] = from.d2; translationMatrix[2][3] = from.d3; translationMatrix[3][3] = from.d4;
		return translationMatrix;
	}

	static inline glm::vec3 GetGLMVec(const aiVector3D& vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}
};
