#ifndef _SHADER_H
#define _SHADER_H

class CShader
{
	protected:
		const char *vertex_src;
		const char *fragment_src;

		GLhandleARB vertex_shader;
		GLhandleARB fragment_shader;
		GLhandleARB program;

		void SetSource(const char *vertex, const char *fragment);
		void CreateVertexShader(void);
		void CreateFragmentShader(void);
		void CreateProgram(void);
		void LinkProgram(void);
	
	public:
		void BindShader(void);
		virtual void ResetUniforms(void) {};

		virtual ~CShader(void) {};
};


void PrintGLInfoLog(const char *log_title, GLhandleARB handle);
GLhandleARB CreateShader(GLenum type, const char *src);
GLhandleARB CreateShaderProgram(GLhandleARB vertex_shader, GLhandleARB fragment_shader);
void LinkShaderProgram(GLhandleARB program);
void UseNoShader(void);

GLuint LoadGLTexture(const char *filename, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);
inline GLuint LoadGLTexture(const char *filename, bool *transparent, int alpha_channel = 3)
						{ return LoadGLTexture(filename, 0, 0, transparent, alpha_channel); }
GLuint GLTextureFromColor(const CVector &color);

#endif
