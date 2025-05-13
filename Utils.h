#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glut/glut.h>
#include <glm/glm.hpp>


#include <iostream>
#include <fstream>
#include <string>



class Utils {
private:
	static GLuint CompileShader(GLuint type, std::string source);
	static std::string readSource(const char* filePath);
public:
	Utils();																										
	static void Clear();
	static GLuint createShaderVF(std::string vertexShader, std::string fragmentShader);
	static GLuint createShaderVFG(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderath);
	static glm::vec3 arrayToVec3(const float* array);
};
