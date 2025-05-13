#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "std_image.h"
#include <string>


class Texture
{
private:
	GLuint m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height;
	int m_BPP;																							// 实际纹理的像素位

public:
	Texture(const std::string& path);
	~Texture();

	void bind(GLuint slot = 0)const;
	void unbind()const;

	inline int getWindth()const { return m_Width; };
	inline int getHeigth()const { return m_Height; };
};