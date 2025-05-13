#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

using namespace std;


class VertexArray
{
private:
	GLuint m_RendererID;																							// 顶点数组指针
public:
	VertexArray();
	~VertexArray();
	void addBuffer(const VertexBuffer &vb, const VertexBufferLayout &Layout);

	void bind() const;
	void unbind() const;
};