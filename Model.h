#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "std_image.h"

#include "Mesh.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory);

class Model {
public:
	
	Model(const char* path)
	{
		this->loadModel(path);

	}

	void Draw(DrawShader& shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(shader);														//////////////////////////////////////////////////
		}
	}

private:

	// Model Data
	vector<TextureModel> textures_loaded;
	vector<Mesh> meshes;
	string directory;

	void loadModel(const string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
			return;
		}

		directory = path.substr(0,path.find_last_of('/'));									// 只是一个相对路径，到模型所在的文件夹


		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode* node, const aiScene* scene)
	{
		// process all the node's meshes
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}


		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
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
			//Position
			Vertex vertex;
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

		// 3. normal maps
		vector<TextureModel> normalMaps = loadMaterialTexture(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		vector<TextureModel> heightMaps = loadMaterialTexture(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());


		return Mesh(vertices, indecies, textures);

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

unsigned int TextureFromFile(const char* path, const string& directory)
{
	GLuint textureID;
	int m_Width, m_Height, m_BPP;
	string fileName = string(path);
	fileName = directory + '/' + fileName;

	stbi_set_flip_vertically_on_load(1);                                                     // 翻转纹理，计算机（纹理）坐标原点在左上角，而OpenGL原点在左下角
	unsigned char* m_LocalBuffer = stbi_load(fileName.c_str(), &m_Width, &m_Height, &m_BPP, 4);					//  返回路径，高度，宽度，像素位值

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);									// 渲染的要比实际纹理小，缩小
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);									// 渲染的要比实际纹理大，放大
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);										// 嵌入式(不希望扩大区域)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);										// ST 相当于XY
	//以上4个必须

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);					// 将数据放入缓冲区
	glGenerateMipmap(GL_TEXTURE_2D);

	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);																						 // 释放掉这个地址，也可以选择保留

	return textureID;
}
