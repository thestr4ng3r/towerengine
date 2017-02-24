
#include "towerengine.h"


#if !defined(BUILD_DEVIL) && !defined(BUILD_LIBPNG)
#pragma message("Warning: Building TowerEngine without any image loading library!")
#endif


#ifdef BUILD_DEVIL

GLuint LoadGLTextureBinary_IL(const char *ext, const void *data, size_t size, int channels, int *w, int *h, bool *transparent);
GLuint LoadGLTexture_IL(const char *filename, int channels, int *w, int *h, bool *transparent);
GLuint LoadGLTextureArray_IL(const char **filenames, const int count, int *w, int *h);

GLuint LoadGLCubeMap_IL(const char *filename[6]);
GLuint LoadGLCubeMap_IL(const char *filename);
GLuint LoadGLCubeMapBinary_IL(const char *ext, const void *data, unsigned int size);

#endif


#ifdef BUILD_LIBPNG

GLuint LoadGLTexture_PNG(const char *filename, int channels, int *w, int *h, bool *transparent);
GLuint LoadGLTextureArray_PNG(const char **filenames, const int count, int *w, int *h);

GLuint LoadGLCubeMap_PNG(const char *filename[6]);
GLuint LoadGLCubeMap_PNG(const char *filename);

#endif




GLuint LoadGLTexture(const char *filename, int channels, int *w, int *h, bool *transparent)
{
#ifdef BUILD_DEVIL
	return LoadGLTexture_IL(filename, channels, w, h, transparent);
#elif BUILD_LIBPNG
	return LoadGLTexture_PNG(filename, channels, w, h, transparent);
#else
	return 0;
#endif
}

GLuint LoadGLTextureBinary(const char *ext, const void *data, size_t size, int channels, int *w, int *h, bool *transparent)
{
#ifdef BUILD_DEVIL
	return LoadGLTextureBinary_IL(ext, data, size, channels, w, h, transparent);
#else
	return 0;
#endif
}

GLuint LoadGLTextureArray(const char **filenames, const int count, int *w, int *h)
{
#ifdef BUILD_DEVIL
	return LoadGLTextureArray_IL(filename, channels, w, h, transparent);
#elif BUILD_LIBPNG
	return LoadGLTextureArray_PNG(filenames, count, w, h);
#else
	return 0;
#endif
}




GLuint LoadGLCubeMap(const char *filename[6])
{
#ifdef BUILD_DEVIL
	return LoadGLCubeMap_IL(filename);
#elif BUILD_LIBPNG
	return LoadGLCubeMap_PNG(filename);
#else
	return 0;
#endif
}

GLuint LoadGLCubeMap(const char *filename)
{
#ifdef BUILD_DEVIL
	return LoadGLCubeMap_IL(filename);
#elif BUILD_LIBPNG
	return LoadGLCubeMap_PNG(filename);
#else
	return 0;
#endif
}


GLuint LoadGLCubeMapBinary(const char *ext, const void *data, unsigned int size)
{
#ifdef BUILD_DEVIL
	return LoadGLCubeMapBinary_IL(ext, data, size);
#else
	return 0;
#endif
}







GLuint CreateCubeTexture(GLenum type, int width, int height)
{
	GLuint tex;

	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(type, tex);
	glTexParameterf(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(type, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(type, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glGenerateMipmap(type);

	return tex;
}

void GetSubImage(GLubyte *dst, GLubyte *src, int src_width, int src_height, int components, int offset_x, int offset_y, int dst_width, int dst_height)
{
	for(int i=0; i<dst_height && i+offset_y<src_height; i++)
	{
		memcpy(dst + i*dst_width*components,
			   src + (i + offset_y) * src_width * components + offset_x * components,
			   dst_width * components * sizeof(GLubyte));
	}
}