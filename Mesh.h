#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "DrawShader.h"

//

using namespace std;
using namespace glm;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

//struct Vertex {
//	vec3 Position;
//	vec3 Normal;
//	vec2 TexCroods;
//};

struct TextureModel
{
	unsigned int id;
	string type;                    // 什么类型的贴图
	string path;
};

class Mesh
{
public:
	// mesh data 
	vector<Vertex> vertices;
	vector<unsigned int> indecies;
	vector<TextureModel> textures;

	// Constructors
	Mesh(vector<Vertex> vertices, vector<unsigned int> indecies, vector<TextureModel> textures)
	{
		this->vertices = vertices;
		this->indecies = indecies;
		this->textures = textures;

		this->setupMesh();
	}

// Draw Shader

	void Draw(DrawShader& shader)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			string name = textures[i].type;
			string number;

			if (name == "texture_diffuse")
				number = to_string(diffuseNr ++);
			else if (name == "texture_specular")
				number = to_string(specularNr ++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to string
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to string

			shader.setUniform1i(("material."+ name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);

		}

//		glActiveTexture(GL_TEXTURE0);

		// Draw Mesh
		glBindVertexArray(this->VAO);
		
		glDrawElements(GL_TRIANGLES, indecies.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}

private:
	// Render Data
	unsigned int VAO, VBO, EBO;

	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indecies.size() * sizeof(unsigned int), &indecies[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// ids
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

		glBindVertexArray(0);
	}

};