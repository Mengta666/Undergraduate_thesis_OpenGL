#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class IndexBuffer
{
private:
	GLuint m_RendererID;
	GLuint m_count;

public:
	IndexBuffer(void* Loction,GLuint count);
	~IndexBuffer();

	void bind() const;
	void unbind() const;

	inline GLuint getCount(){ return m_count; };
};
