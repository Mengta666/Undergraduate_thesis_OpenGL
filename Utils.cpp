#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glut/glut.h>


#include <iostream>
#include <fstream>
#include <string>
#include "Utils.h"

using namespace std;

int result;

Utils::Utils() {};

// ��ȡglsl�ļ�
std::string Utils::readSource(const char* filePath)
{
    string content;
    ifstream fileStream(filePath, ios::in);                                                     //ʵ����
    if (!fileStream.good())
    {
        std::cout << "\nERROR��filePath, info: " << +filePath << + "\n";
        exit(NULL);
    }
    string line = "";
    while (!fileStream.eof())
    {
        getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}

// ��Shader���������תΪ�ַ����������б���
GLuint Utils::CompileShader(GLuint type, std::string source)
{
    string str = readSource(source.c_str());
    GLuint id = glCreateShader(type);                                                          //����һ���յ���ɫ������
    const char* src = str.c_str();
    glShaderSource(id, 1, &src, NULL);                                                          //�� id �е�Դ��������Ϊ src ָ�����ַ��������е�Դ���롣
    glCompileShader(id);                                                                        //�����Ѵ洢�� Shader ָ���� Shader �����е�Դ�����ַ�����

//  ������
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(sizeof(char) * length);                                    // alloca����Ҫ�ֶ��ͷ��ڴ�
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader" << std::endl;
        std::cout << message << std::endl;
    }

    return id;

}

//������ɫ��Program���󣬽������������󷵻�
GLuint Utils::createShaderVF(std::string vertexShader, std::string fragmentShader)
{
    GLuint program = glCreateProgram();                                                         //����һ���յ�Program����
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);                              //����glslԴ�ļ�

    glAttachShader(program, vs);
    glAttachShader(program, fs);                                                          //����ɫ��ָ������ɫ�����󸽼ӵ�����ָ���ĳ������
    glLinkProgram(program);                                                              //����κ����͵���ɫ�����󸽼ӵ��������ǽ����ڴ������ڿɱ�̶��㴦���������еĿ�ִ���ļ�

    glValidateProgram(program);                                                          //�������а����Ŀ�ִ���ļ��Ƿ�����ڸ�����ǰ OpenGL ״̬�������ִ��


    return program;
}

GLuint Utils::createShaderVFG(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderath)
{
    GLuint program = glCreateProgram();                                                         //����һ���յ�Program����
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShaderPath);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderPath);                              
    GLuint gs = CompileShader(GL_GEOMETRY_SHADER, geometryShaderath);                                       //����glslԴ�ļ�

    glAttachShader(program, vs);
    glAttachShader(program, fs);                                                          //����ɫ��ָ������ɫ�����󸽼ӵ�����ָ���ĳ������
    glAttachShader(program, gs);
    glLinkProgram(program);                                                              //����κ����͵���ɫ�����󸽼ӵ��������ǽ����ڴ������ڿɱ�̶��㴦���������еĿ�ִ���ļ�

    glValidateProgram(program);                                                          //�������а����Ŀ�ִ���ļ��Ƿ�����ڸ�����ǰ OpenGL ״̬�������ִ��


    return program;
}

void Utils::Clear()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);                                                                       //ÿ�λ���ǰ���������ɫ������Ϊ��ɫ
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::vec3 Utils::arrayToVec3(const float* array)
{
    glm::vec3 vec3 = glm::vec3(array[0], array[1], array[2]);
    return vec3;
}
