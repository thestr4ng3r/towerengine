#include "towerengine.h"


using namespace std;


tVAO::tVAO(void)
{
	glGenVertexArrays(1, &vao);
}

tVAO::~tVAO(void)
{
	glDeleteVertexArrays(1, &vao);
}

void tVAO::Draw(GLenum mode, GLint first, GLsizei count)
{
	Bind();
	glDrawArrays(mode, first, count);
	UnBind();
}


// --------------------------------------------------------------------


tIBO::tIBO(int size)
{
	data = 0;
	this->size = 0;
	//vao->Bind();
	glGenBuffers(1, &ibo);
	//tVAO::UnBind();
	SetSize(size);
}

tIBO::~tIBO(void)
{
	glDeleteBuffers(1, &ibo);
}

bool tIBO::SetSize(int size, bool copy)
{
	if(size == this->size && data != 0)
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

	AssignData();
	return true;
}

void tIBO::AssignData(void)
{
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * size, data, GL_DYNAMIC_DRAW);
	UnBind();
}

void tIBO::Draw(GLenum mode)
{
	Bind();
	glDrawElements(mode, size, GL_UNSIGNED_INT, NULL);
	UnBind();
}
