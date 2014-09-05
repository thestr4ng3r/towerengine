#ifndef _SHADER_H
#define _SHADER_H

struct CShaderProgram
{
	const char *vertex_src;
	const char *fragment_src;

	GLhandleARB vertex_shader;
	GLhandleARB fragment_shader;
	GLhandleARB program;
};

class CShader : protected CShaderProgram
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
		void Bind(void);
		static void Unbind(void);

		virtual void ResetUniforms(void) {};

		virtual ~CShader(void) {};
};


void PrintGLInfoLog(const char *log_title, GLhandleARB handle);
GLhandleARB CreateShader(GLenum type, const char *src, const char *name);
GLhandleARB CreateShaderProgram(GLhandleARB vertex_shader, GLhandleARB fragment_shader);
void LinkShaderProgram(GLhandleARB program);

GLuint LoadGLTexture(const char *filename, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);
GLuint LoadGLTextureBinary(const char *ext, const void *data, unsigned int size, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);
GLuint LoadGLTextureIL(ILuint image, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);

inline GLuint LoadGLTexture(const char *filename, bool *transparent, int alpha_channel = 3)
		{ return LoadGLTexture(filename, 0, 0, transparent, alpha_channel); }
inline GLuint LoadGLTextureBinary(const char *ext, const void *data, unsigned int size, bool *transparent, int alpha_channel = 3)
		{ return LoadGLTextureBinary(ext, data, size, 0, 0, transparent, alpha_channel); }

GLuint GLTextureFromColor(const CVector &color);

#endif
