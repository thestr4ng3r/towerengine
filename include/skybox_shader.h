#ifndef _SKYBOX_SHADER_H
#define _SKYBOX_SHADER_H

class tSkyBoxShader : public tShader
{
	private:
		GLint cam_pos_uniform;

		GLint tex_uniform;
	
	public:
		static const int vertex_attribute = 0;

		tSkyBoxShader(void);
		void SetCubeMap(GLuint tex);
		void SetCameraPosition(tVector pos);
};

GLenum CubeTex(int side);
int CubeSide(GLenum tex);
tVector CubeVecS(int side);

//GLuint LoadGLTexture(const char *filename, GLenum type = GL_TEXTURE_2D);
GLuint LoadGLCubeMap(const char *filename[6]);
GLuint LoadGLCubeMap(const char *filename);
GLuint LoadGLCubeMapBinary(const char *ext, const void *data, unsigned int size);

GLuint CreateCubeTexture(GLenum type, int width, int height);
GLuint GLTextureFromColor(const tVector &color);

#endif
