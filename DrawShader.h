#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <unordered_map>                                              //无序哈希表

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"


class DrawShader
{
private:
	GLuint Shader;
	GLint getUniformLocation(const std::string& location);
//	std::unordered_map <const std::string, int> m_UniformLocationCache;													// 创建一个哈希键值表,加了const报错
	std::unordered_map <std::string, int> m_UniformLocationCache;													// 创建一个哈希键值表

public:
	DrawShader(std::string vertexShaderPath, std::string fragmentShaderPath);
	DrawShader(string vertexShaderPath, string fragmentShaderPath, string geometryShaderath);
	~DrawShader();

	void setUniformBoolen(const string& name, bool v0);
	void setUniform1i(const std::string& name, int v0);
	void setUniform1f(const std::string& name, float v0);
	void setUniform3f(const string& name, float v0, float v1, float v2);
	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

	void setUniformVec2(const string& name, glm::vec2 value);
	void setUniformVec3(const string& name, glm::vec3 value);
	void setUniformVec4(const string& name, glm::vec4 value);

	void setUniformMat4f(const string& name, const glm::mat4 matrix);

	void bind()const;
	void unbind()const;
	void display(int count)const;
	void displays(int count)const;
};

//GLuint Shader = Utils::createShader("Shaders/vertexShader.glsl", "Shaders/fragmentShader.glsl");