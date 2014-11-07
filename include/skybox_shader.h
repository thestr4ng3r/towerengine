#ifndef _ENVIRONMENT_SHADER_H
#define _ENVIRONMENT_SHADER_H

class tSkyBoxShader : public tShader
{
	private:
		GLint tex_uniform;
	
	public:
		static const int vertex_attribute = 0;

		void Init(void);
		void SetCubeMap(GLuint tex);
		void ResetUniforms(void) {};
};

GLenum CubeTex(int side);
int CubeSide(GLenum tex);
tVector CubeVecS(int side);

//GLuint LoadGLTexture(const char *filename, GLenum type = GL_TEXTURE_2D);
GLuint LoadGLCubeMap(const char *filename[6]);
GLuint LoadGLCubeMap(const char *filename);
GLuint LoadGLCubeMapBinary(const char *ext, const void *data, unsigned int size);
GLuint LoadGLCubeMapIL(ILuint image);

GLuint CreateCubeTexture(GLenum type, int width, int height);
GLuint GLTextureFromColor(const tVector &color);

#endif
