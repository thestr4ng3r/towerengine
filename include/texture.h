
#ifndef _TEXTURE_H
#define _TEXTURE_H

GLuint LoadGLTexture(const char *filename, int channels = 4, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);
GLuint LoadGLTextureBinary(const char *ext, const void *data, size_t size, int channels = 4, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);

GLuint LoadGLTextureArray(const char **filenames, const int count, int *w = 0, int *h = 0);

inline GLuint LoadGLTexture(const char *filename, int channels, bool *transparent, int alpha_channel = 3)
{
	return LoadGLTexture(filename, channels, 0, 0, transparent, alpha_channel);
}

inline GLuint LoadGLTextureBinary(const char *ext, const void *data, size_t size, int channels, bool *transparent, int alpha_channel = 3)
{
	return LoadGLTextureBinary(ext, data, size, channels, 0, 0, transparent, alpha_channel);
}


GLuint LoadGLCubeMap(const char *filename[6]);
GLuint LoadGLCubeMap(const char *filename);
GLuint LoadGLCubeMapBinary(const char *ext, const void *data, unsigned int size);

GLuint CreateCubeTexture(GLenum type, int width, int height);
GLuint GLTextureFromColor(const tVector &color);
void GetSubImage(GLubyte *dst, GLubyte *src, int src_width, int src_height, int components, int offset_x, int offset_y, int dst_width, int dst_height);

#endif
