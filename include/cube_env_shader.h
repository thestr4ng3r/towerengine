#ifndef _ENVIRONMENT_SHADER_H
#define _ENVIRONMENT_SHADER_H

class CCubeEnvShader : public CShader
{
	private:
		GLint tex_uniform;
	
	public:
		void Init(void);
		void SetCubeMap(GLuint tex);
		void ResetUniforms(void) {};
};

extern CCubeEnvShader cube_env_shader;

GLenum CubeTex(int side);
int CubeSide(GLenum tex);
CVector CubeVecS(int side);

//GLuint LoadGLTexture(const char *filename, GLenum type = GL_TEXTURE_2D);
GLuint LoadGLCubeMap(const char *filename[6]);

GLuint CreateCubeTexture(GLenum type, int width, int height);
GLuint GLTextureFromColor(const CVector &color);

#endif
