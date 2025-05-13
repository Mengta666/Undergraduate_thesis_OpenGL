#include "Texture.h"


Texture::Texture(const std::string& path)
	: m_Height(0), m_Width(0), m_FilePath(path), 
	m_LocalBuffer(NULL), m_BPP(0), m_RendererID(0)
{
	stbi_set_flip_vertically_on_load(1);                                                     // 翻转纹理，计算机（纹理）坐标原点在左上角，而OpenGL原点在左下角
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);					//  返回路径，高度，宽度，像素位值，RGBA型（4）

	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);									// 渲染的要比实际纹理小，缩小
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);									// 渲染的要比实际纹理大，放大
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);										// 嵌入式(不希望扩大区域)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);										// ST 相当于XY
	//以上4个必须

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);					// 将数据放入缓冲区
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);																	//解绑

	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);																						 // 释放掉这个地址，也可以选择保留
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_RendererID);
}

void Texture::bind(GLuint slot)const
{
	glActiveTexture(GL_TEXTURE0 + slot);																	// 绑定插槽
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::unbind()const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}