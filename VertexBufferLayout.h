#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

using namespace std;

struct VertexBufferElement
{	
	GLuint type;														// ���Ƶ���������
	GLuint count;														// һ�������м�������
	GLbyte normalied;													// �Ƿ񱻹�һ��

	static GLuint getSizeOfBuffer(GLuint type)
	{
		switch (type)
		{

			case GL_FLOAT:
				return sizeof(GLfloat);
				break;
			case GL_UNSIGNED_INT:
				return sizeof(GLuint);
				break;
			case GL_UNSIGNED_BYTE:
				return sizeof(GLbyte);
				break;
			default:
				return 0;

		}
			
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;													// ������Ϊm_Elements�Ľṹ�����û�г�ʼ��
	GLuint m_Stride;																			    // ����,��help.txt
public:
	VertexBufferLayout()
	{
		m_Stride = 0;
	}

	template <typename T>
	void Push(GLuint count)
	{
		cout << "no thing to do" << endl;
	}

	template<>
	void Push<GLfloat>(GLuint count)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE});											// ֱ���ڽṹ������������һ���ṹ��
		m_Stride += sizeof(GLfloat) * count;														
	}

	template<>
	void Push<GLuint>(GLuint count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += sizeof(GLuint) * count;																// ��help.txt 2
	}

	template<>
	void Push<GLbyte>(GLuint count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += sizeof(GLbyte) * count;
	}

	inline std::vector<VertexBufferElement> GetElements()const { return m_Elements; };				// ��������
	inline GLuint GetStride() const { return m_Stride; };
};
