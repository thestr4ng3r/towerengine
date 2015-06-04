#ifndef _SHADER_H
#define _SHADER_H

struct tShaderProgram
{
	const char *vertex_src;
	const char *fragment_src;

	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;
};

class tShader : protected tShaderProgram
{
	private:
		const char *name;

	protected:
		void SetSource(const char *vertex, const char *fragment);
		void CreateVertexShader(void);
		void CreateFragmentShader(void);
		void CreateProgram(void);
		void LinkProgram(void);

		void InitShader(const char *vert_src, const char *frag_src, const char *shader_name = 0);
		GLint GetUniformLocation(const char *uniform);
	
	public:
		static const int vertex_attribute = 0;

		void Bind(void);
		static void Unbind(void);

		virtual ~tShader(void) {};
};


void PrintGLInfoLog(const char *log_title, GLuint handle);
GLuint CreateShader(GLenum type, const char *src, const char *name);
GLuint CreateShaderProgram(GLuint vertex_shader, GLuint fragment_shader);
void LinkShaderProgram(GLuint program);

GLuint LoadGLTexture(const char *filename, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);
GLuint LoadGLTextureBinary(const char *ext, const void *data, unsigned int size, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);

inline GLuint LoadGLTexture(const char *filename, bool *transparent, int alpha_channel = 3)
		{ return LoadGLTexture(filename, 0, 0, transparent, alpha_channel); }
inline GLuint LoadGLTextureBinary(const char *ext, const void *data, unsigned int size, bool *transparent, int alpha_channel = 3)
		{ return LoadGLTextureBinary(ext, data, size, 0, 0, transparent, alpha_channel); }

//GLuint GLTextureFromColor(const tVector &color);

#endif
