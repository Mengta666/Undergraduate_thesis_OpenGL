#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "VertexArray.h"


VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_RendererID);																		// 创建顶点数组指针,当前只是相当于一个空指针
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::bind() const
{
	glBindVertexArray(m_RendererID);																			// 绑定顶点数组
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);																						// 解除绑定
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& Layout)
{
	bind();																												// 绑定顶点数组
	vb.bind();																											// 绑定缓冲区
	const auto& elements = Layout.GetElements();
	GLuint offset = 0;														//指定当前绑定到目标的缓冲区的数据存储区中数组中第一个泛型顶点属性的第一个分量的偏移量。
	for (GLuint i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glVertexAttribPointer(i, element.count, element.type, element.normalied, Layout.GetStride(), (void*) offset);
		offset += element.count * element.getSizeOfBuffer(element.type);												
							// 计算某buffer长度,在vao中确定下一个缓冲区头部离头部的位置，图见help.txt
		glEnableVertexAttribArray(i);																			// 将缓冲区依次放入vao指针中，见help.txt
	}
	
}