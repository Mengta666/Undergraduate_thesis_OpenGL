#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "std_image.h"

#include "AssimpGLMHelpers.h"
#include "Mesh.h"


#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory);

struct BoneInfo
{
	/*需要进行变换的骨骼id*/
	int id;

	/*模型变换的偏移矩阵（与骨骼id绑定）*/
	glm::mat4 offset;

};

class Model_Animation {
public:

	Model_Animation(const char* path)
	{
		this->loadModel(path);

	}

	void Draw(DrawShader& shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(shader);														
		}
	}

	auto& GetBoneInfoMap() { return BoneInfoMap; }
	int& GetBoneCount() { return BoneCounter; }

private:

	// Model Data
	vector<TextureModel> textures_loaded;
	vector<Mesh> meshes;
	string directory;

	// 骨骼相关
	std::map<string, BoneInfo> BoneInfoMap;
	int BoneCounter = 0;

	void loadModel(const string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
			exit(NULL);
		}

		directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode* node, const aiScene* scene)
	{
		// 遍历所有的父节点
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}


		// 寻找父节点的其他子节点
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	/* 初始化骨骼节点相邻的其他节点，将其置为没有被遍历的状态 */
	void SetVertexBoneDataToDefault(Vertex& vertex)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
		{
			vertex.m_BoneIDs[i] = -1;
			vertex.m_Weights[i] = 0.0f;
		}
	}

	/* 遍历与该骨骼节点相邻且权重占比最大的几个相邻节点，找到的设置为遍历状态 */
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
		{
			if (vertex.m_BoneIDs[i] < 0)
			{
				vertex.m_Weights[i] = weight;
				vertex.m_BoneIDs[i] = boneID;
				break;
			}
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		//data
		vector<Vertex> vertices;
		vector<unsigned int> indecies;
		vector<TextureModel> textures;

		//vertex
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			SetVertexBoneDataToDefault(vertex);

			//Position
			glm::vec3 vector3;
			vector3.x = mesh->mVertices[i].x;
			vector3.y = mesh->mVertices[i].y;
			vector3.z = mesh->mVertices[i].z;
			vertex.Position = vector3;

			//Normals
			if (mesh->HasNormals())
			{
				vector3.x = mesh->mNormals[i].x;
				vector3.y = mesh->mNormals[i].y;
				vector3.z = mesh->mNormals[i].z;

				vertex.Normal = vector3;
			}

			//TexCroods
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vector2;
				vector2.x = mesh->mTextureCoords[0][i].x;
				vector2.y = mesh->mTextureCoords[0][i].y;

				vertex.TexCoords = vector2;

				// tangent
				vector3.x = mesh->mTangents[i].x;
				vector3.y = mesh->mTangents[i].y;
				vector3.z = mesh->mTangents[i].z;
				vertex.Tangent = vector3;
				// bitangent
				vector3.x = mesh->mBitangents[i].x;
				vector3.y = mesh->mBitangents[i].y;
				vector3.z = mesh->mBitangents[i].z;
				vertex.Bitangent = vector3;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f);

			vertices.push_back(vertex);
		}

		//Indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indecies.push_back(face.mIndices[j]);
		}

		//Texture
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		//diffuse map
		vector<TextureModel> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		//specuar map
		vector<TextureModel> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// normal maps
		vector<TextureModel> normalMaps = loadMaterialTexture(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// height maps
		vector<TextureModel> heightMaps = loadMaterialTexture(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		ExtractBoneWeightForVertices(vertices, mesh, scene);

		return Mesh(vertices, indecies, textures);

	}

	// 骨骼权重绑定
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
	{
		auto& boneInfoMap = BoneInfoMap;
		int& boneCount = BoneCounter;

		for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			int boneID = -1;
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				BoneInfo newBoneInfo;
				newBoneInfo.id = boneCount;
				// glm与Assimp存储矩阵的方式不一样，需做一次转换
				newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMMatrix(mesh->mBones[boneIndex]->mOffsetMatrix);
				boneInfoMap[boneName] = newBoneInfo;
				boneID = boneCount;
				boneCount++;
			}
			else
			{
				boneID = boneInfoMap[boneName].id;
			}
			assert(boneID != -1);
			auto weights = mesh->mBones[boneIndex]->mWeights;
			int numWeights = mesh->mBones[boneIndex]->mNumWeights;
			for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
			{
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				assert(vertexId <= vertices.size());
				SetVertexBoneData(vertices[vertexId], boneID, weight);
			}
		}
	}


	vector<TextureModel> loadMaterialTexture(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<TextureModel> textures;

		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			bool skip = false;

			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				TextureModel texture;
				texture.id = TextureFromFile(str.C_Str(), directory);
				texture.path = str.C_Str();
				texture.type = typeName;
				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}
		}

		return textures;
	}

};



