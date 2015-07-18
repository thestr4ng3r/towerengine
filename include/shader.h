#ifndef _SHADER_H
#define _SHADER_H

class tShader
{
	private:
		const char *name;

	protected:
		GLuint program;

		void CreateAndAttachShader(GLenum type, const char *src);
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

GLuint LoadGLTexture(const char *filename, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);
GLuint LoadGLTextureBinary(const char *ext, const void *data, unsigned int size, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);

GLuint LoadGLTextureArray(const char **filenames, int count, int *w = 0, int *h = 0);

inline GLuint LoadGLTexture(const char *filename, bool *transparent, int alpha_channel = 3)
		{ return LoadGLTexture(filename, 0, 0, transparent, alpha_channel); }
inline GLuint LoadGLTextureBinary(const char *ext, const void *data, unsigned int size, bool *transparent, int alpha_channel = 3)
		{ return LoadGLTextureBinary(ext, data, size, 0, 0, transparent, alpha_channel); }


#endif
