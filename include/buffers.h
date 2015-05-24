#ifndef _BUFFERS_H
#define _BUFFERS_H

class tVAO
{
private:
	GLuint vao;

public:
	tVAO(void);
	~tVAO(void);

	void Bind(void)					{ glBindVertexArray(vao); };
	static void UnBind(void)		{ glBindVertexArray(0); };
	void Draw(GLenum mode, GLint first, GLsizei count);
};

template <class T> class tVBO
{
private:
	T *data;
	int size;
	int components;
	tVAO *vao;

	GLuint vbo;

public:
	tVBO(int components, tVAO *vao, int size = 0);
	~tVBO(void);

	void SetAttribute(GLuint index, GLenum type, GLboolean normalize = GL_FALSE, GLsizei stride = 0);
	void AssignData(void);
	void Bind(void)					{ glBindBuffer(GL_ARRAY_BUFFER, vbo); };
	static void UnBind(void)		{ glBindBuffer(GL_ARRAY_BUFFER, 0); };
	bool SetSize(int size, bool copy = false); // returns true if size has changed and false if not
	T *GetData(void)				{ return data; };
	int GetComponents(void)			{ return components; };
	int GetSize(void)				{ return size; };
};


class tIBO
{
private:
	GLuint ibo;
	GLuint *data;
	int size;

public:
	tIBO(tVAO *vao, int size = 0);
	~tIBO(void);

	void SetAttribute(GLuint index, GLenum type, GLboolean normalize = GL_FALSE, GLsizei stride = 0);
	void Bind(void)					{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); };
	static void UnBind(void)		{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); };
	bool SetSize(int size, bool copy = false); // returns true if size has changed and false if not
	GLuint *GetData(void)			{ return data; };
	int GetSize(void)				{ return size; };
	void AssignData(void);
	void Draw(GLenum mode);
};








template <class T> tVBO<T>::tVBO(int components, tVAO *vao, int size)
{
	this->components = components;
	this->size = 0;
	data = 0;

	vao->Bind();
	glGenBuffers(1, &vbo);
	tVAO::UnBind();
	this->vao = vao;

	SetSize(size);
}

template <class T> tVBO<T>::~tVBO(void)
{
	glDeleteBuffers(1, &vbo);
	if(data != 0)
		delete [] data;
}

template <class T> void tVBO<T>::SetAttribute(GLuint index, GLenum type, GLboolean normalize, GLsizei stride)
{
	Bind();
	glVertexAttribPointer(index, components, type, normalize, stride, 0);
	glEnableVertexAttribArray(index);
	UnBind();
}

template <class T> void tVBO<T>::AssignData(void)
{
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size * components * sizeof(T), data, GL_DYNAMIC_DRAW);
	UnBind();
}

template <class T> bool tVBO<T>::SetSize(int size, bool copy)
{
	if(size == this->size)
		return false;

	T *old = data;

	data = new T[size * components];

	if(old != 0)
	{
		if(copy)
			memcpy(data, old, std::min(this->size, size) * components * sizeof(T));
		delete[] old;
	}
	this->size = size;
	return true;
}

#endif
