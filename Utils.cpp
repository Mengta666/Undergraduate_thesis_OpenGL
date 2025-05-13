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

// 读取glsl文件
std::string Utils::readSource(const char* filePath)
{
    string content;
    ifstream fileStream(filePath, ios::in);                                                     //实例化
    if (!fileStream.good())
    {
        std::cout << "\nERROR：filePath, info: " << +filePath << + "\n";
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

// 将Shader对象的内容转为字符串，并进行编译
GLuint Utils::CompileShader(GLuint type, std::string source)
{
    string str = readSource(source.c_str());
    GLuint id = glCreateShader(type);                                                          //创建一个空的着色器对象
    const char* src = str.c_str();
    glShaderSource(id, 1, &src, NULL);                                                          //将 id 中的源代码设置为 src 指定的字符串数组中的源代码。
    glCompileShader(id);                                                                        //编译已存储在 Shader 指定的 Shader 对象中的源代码字符串。

//  错误检查
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(sizeof(char) * length);                                    // alloca不需要手动释放内存
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader" << std::endl;
        std::cout << message << std::endl;
    }

    return id;

}

//创建着色器Program对象，将编译结果关联后返回
GLuint Utils::createShaderVF(std::string vertexShader, std::string fragmentShader)
{
    GLuint program = glCreateProgram();                                                         //创建一个空的Program对象
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);                              //编译glsl源文件

    glAttachShader(program, vs);
    glAttachShader(program, fs);                                                          //将着色器指定的着色器对象附加到程序指定的程序对象。
    glLinkProgram(program);                                                              //如果任何类型的着色器对象附加到程序，它们将用于创建将在可编程顶点处理器上运行的可执行文件

    glValidateProgram(program);                                                          //检查程序中包含的可执行文件是否可以在给定当前 OpenGL 状态的情况下执行


    return program;
}

GLuint Utils::createShaderVFG(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderath)
{
    GLuint program = glCreateProgram();                                                         //创建一个空的Program对象
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShaderPath);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderPath);                              
    GLuint gs = CompileShader(GL_GEOMETRY_SHADER, geometryShaderath);                                       //编译glsl源文件

    glAttachShader(program, vs);
    glAttachShader(program, fs);                                                          //将着色器指定的着色器对象附加到程序指定的程序对象。
    glAttachShader(program, gs);
    glLinkProgram(program);                                                              //如果任何类型的着色器对象附加到程序，它们将用于创建将在可编程顶点处理器上运行的可执行文件

    glValidateProgram(program);                                                          //检查程序中包含的可执行文件是否可以在给定当前 OpenGL 状态的情况下执行


    return program;
}

void Utils::Clear()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);                                                                       //每次绘制前清除背景颜色，重置为灰色
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::vec3 Utils::arrayToVec3(const float* array)
{
    glm::vec3 vec3 = glm::vec3(array[0], array[1], array[2]);
    return vec3;
}
