#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "IndexBuffer.h"

using namespace std;


IndexBuffer::IndexBuffer(void* Loction, GLuint count)
{
	m_count = count;
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, Loction, GL_STATIC_DRAW);                        //°ó¶¨Ë÷Òý»º³åÇø
//	cout << "m_count = " << m_count << endl;
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_RendererID);
}

void IndexBuffer::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}