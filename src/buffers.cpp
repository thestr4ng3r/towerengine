#include "towerengine.h"

VAO::VAO(void)
{
	glGenVertexArrays(1, &vao);
}

VAO::~VAO(void)
{
	glDeleteVertexArrays(1, &vao);
}

void VAO::Draw(GLenum mode, GLint first, GLsizei count)
{
	Bind();
	glDrawArrays(mode, first, count);
	UnBind();
}


// --------------------------------------------------------------------


IBO::IBO(VAO *vao, int size)
{
	data = 0;
	this->size = 0;
	vao->Bind();
	glGenBuffers(1, &ibo);
	VAO::UnBind();
	SetSize(size);
}

IBO::~IBO(void)
{
	glDeleteBuffers(1, &ibo);
}

bool IBO::SetSize(int size, bool copy)
{
	if(size == this->size)
		return false;

	GLuint *old = data;

	data = new GLuint[size];

	if(old != 0)
	{
		if(copy)
			memcpy(data, old, min(this->size, size) * sizeof(GLuint));
		delete[] old;
	}
	this->size = size;

	//AssignData();
	return true;
}

void IBO::AssignData(void)
{
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * size, data, GL_STATIC_DRAW);
	UnBind();
}

void IBO::Draw(GLenum mode)
{
	Bind();
	glDrawElements(mode, size, GL_UNSIGNED_INT, NULL);
	UnBind();
}
