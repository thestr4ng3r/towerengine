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

		GLuint vbo;

	public:
		tVBO(int components, int size = 0);
		~tVBO(void);

		void SetAttribute(GLuint index, GLenum type, GLboolean normalize = GL_FALSE, GLsizei stride = 0);
		void AssignData(void);
		void Bind(void)					{ glBindBuffer(GL_ARRAY_BUFFER, vbo); };
		static void UnBind(void)		{ glBindBuffer(GL_ARRAY_BUFFER, 0); };
		bool SetSize(int size); // returns true if size has changed and false if not
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
		tIBO(int size = 0);
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








template <class T> tVBO<T>::tVBO(int components, int size)
{
	this->components = components;
	this->size = 0;
	data = 0;

	glGenBuffers(1, &vbo);

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
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, components, type, normalize, stride, 0);
	UnBind();
}

template <class T> void tVBO<T>::AssignData(void)
{
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size * components * sizeof(T), data, GL_DYNAMIC_DRAW);
	UnBind();
}

template <class T> bool tVBO<T>::SetSize(int size)
{
	if(size == this->size)
		return false;

	T *old = data;

	data = new T[size * components];

	if(old)
		delete[] old;

	this->size = size;
	return true;
}

#endif
