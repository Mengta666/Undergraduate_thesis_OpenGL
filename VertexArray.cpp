#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "VertexArray.h"


VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_RendererID);																		// ������������ָ��,��ǰֻ���൱��һ����ָ��
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::bind() const
{
	glBindVertexArray(m_RendererID);																			// �󶨶�������
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);																						// �����
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& Layout)
{
	bind();																												// �󶨶�������
	vb.bind();																											// �󶨻�����
	const auto& elements = Layout.GetElements();
	GLuint offset = 0;														//ָ����ǰ�󶨵�Ŀ��Ļ����������ݴ洢���������е�һ�����Ͷ������Եĵ�һ��������ƫ������
	for (GLuint i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glVertexAttribPointer(i, element.count, element.type, element.normalied, Layout.GetStride(), (void*) offset);
		offset += element.count * element.getSizeOfBuffer(element.type);												
							// ����ĳbuffer����,��vao��ȷ����һ��������ͷ����ͷ����λ�ã�ͼ��help.txt
		glEnableVertexAttribArray(i);																			// �����������η���vaoָ���У���help.txt
	}
	
}