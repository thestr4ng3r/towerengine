#ifndef _BUFFERS_H
#define _BUFFERS_H

class VAO
{
private:
	GLuint vao;

public:
	VAO(void);
	~VAO(void);

	void Bind(void)					{ glBindVertexArray(vao); };
	static void UnBind(void)		{ glBindVertexArray(0); };
	void Draw(GLenum mode, GLint first, GLsizei count);
};

template <class T> class VBO
{
private:
	T *data;
	int size;
	int components;
	VAO *vao;

	GLuint vbo;

public:
	VBO(int components, VAO *vao, int size = 0);
	~VBO(void);

	void SetAttribute(GLuint index, GLenum type, GLboolean normalize = GL_FALSE, GLsizei stride = 0);
	void AssignData(void);
	void Bind(void)					{ glBindBuffer(GL_ARRAY_BUFFER, vbo); };
	static void UnBind(void)		{ glBindBuffer(GL_ARRAY_BUFFER, 0); };
	bool SetSize(int size, bool copy = false); // returns true if size has changed and false if not
	T *GetData(void)				{ return data; };
	int GetComponents(void)			{ return components; };
	int GetSize(void)				{ return size; };
};


class IBO
{
private:
	GLuint ibo;
	GLuint *data;
	int size;

public:
	IBO(VAO *vao, int size = 0);
	~IBO(void);

	void SetAttribute(GLuint index, GLenum type, GLboolean normalize = GL_FALSE, GLsizei stride = 0);
	void Bind(void)					{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); };
	static void UnBind(void)		{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); };
	bool SetSize(int size, bool copy = false); // returns true if size has changed and false if not
	GLuint *GetData(void)			{ return data; };
	int GetSize(void)				{ return size; };
	void AssignData(void);
	void Draw(GLenum mode);
};








template <class T> VBO<T>::VBO(int components, VAO *vao, int size)
{
	this->components = components;
	this->size = 0;
	data = 0;

	vao->Bind();
	glGenBuffers(1, &vbo);
	VAO::UnBind();
	this->vao = vao;

	SetSize(size);
}

template <class T> VBO<T>::~VBO(void)
{
	glDeleteBuffers(1, &vbo);
	if(data != 0)
		delete [] data;
}

template <class T> void VBO<T>::SetAttribute(GLuint index, GLenum type, GLboolean normalize, GLsizei stride)
{
	Bind();
	glVertexAttribPointer(index, components, type, normalize, stride, 0);
	glEnableVertexAttribArray(index);
	UnBind();
}

template <class T> void VBO<T>::AssignData(void)
{
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size * components * sizeof(T), data, GL_DYNAMIC_DRAW);
	UnBind();
}

template <class T> bool VBO<T>::SetSize(int size, bool copy)
{
	if(size == this->size)
		return false;

	T *old = data;

	data = new T[size * components];

	if(old != 0)
	{
		if(copy)
			memcpy(data, old, min(this->size, size) * components * sizeof(T));
		delete[] old;
	}
	this->size = size;
	return true;
}

#endif
