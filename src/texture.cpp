
#include "towerengine.h"



#ifdef BUILD_DEVIL

#include <IL/il.h>
#include <IL/ilu.h>

GLuint LoadGLTexture_IL_Image(ILuint image, int channels, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);
GLuint LoadGLTextureBinary_IL(const char *ext, const void *data, size_t size, int channels, int *w, int *h, bool *transparent, int alpha_channel);
GLuint LoadGLTexture_IL(const char *filename, int channels, int *w, int *h, bool *transparent, int alpha_channel);
GLuint LoadGLTextureArray_IL(const char **filenames, const int count, int *w, int *h);

GLuint LoadGLCubeMap_IL_Image(ILuint image);
GLuint LoadGLCubeMap_IL(const char *filename[6]);
GLuint LoadGLCubeMap_IL(const char *filename);
GLuint LoadGLCubeMapBinary_IL(const char *ext, const void *data, unsigned int size);

#endif




GLuint LoadGLTexture(const char *filename, int channels, int *w, int *h, bool *transparent, int alpha_channel)
{
#ifdef BUILD_DEVIL
	return LoadGLTexture_IL(filename, channels, w, h, transparent, alpha_channel);
#else
	return 0;
#endif
}

GLuint LoadGLTextureBinary(const char *ext, const void *data, size_t size, int channels, int *w, int *h, bool *transparent, int alpha_channel)
{
#ifdef BUILD_DEVIL
	return LoadGLTextureBinary_IL(ext, data, size, channels, w, h, transparent, alpha_channel);
#else
	return 0;
#endif
}

GLuint LoadGLTextureArray(const char **filenames, const int count, int *w, int *h)
{
#ifdef BUILD_DEVIL
	return LoadGLTextureArray_IL(filename, channels, w, h, transparent, alpha_channel);
#else
	return 0;
#endif
}




GLuint LoadGLCubeMap(const char *filename[6])
{
#ifdef BUILD_DEVIL
	return LoadGLCubeMap_IL(filename);
#else
	return 0;
#endif
}

GLuint LoadGLCubeMap(const char *filename)
{
#ifdef BUILD_DEVIL
	return LoadGLCubeMap_IL(filename);
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


#ifdef BUILD_DEVIL

GLuint LoadGLTexture_IL(const char *filename, int channels, int *w, int *h, bool *transparent, int alpha_channel)
{
	ILuint imageID;
	ILboolean success;
	ILenum error;
	GLuint r_tex;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	success = ilLoadImage(filename);

	if (!success)
	{
		error = ilGetError();
		printf("Failed to load image \"%s\": %s\n", filename, iluErrorString(error));
		return 0;
	}

	r_tex = LoadGLTexture_IL_Image(imageID, channels, w, h, transparent, alpha_channel);

	ilDeleteImages(1, &imageID);

	return r_tex;
}

GLuint LoadGLTexture_IL_Binary(const char *ext, const void *data, size_t size, int channels, int *w, int *h, bool *transparent, int alpha_channel)
{
	ILuint imageID;
	ILboolean success;
	ILenum error;
	GLuint r_tex;

	ILenum file_type = IL_TYPE_UNKNOWN;

	if(ext)
		file_type = ilTypeFromExt((string("f.") + string(ext)).c_str());

	if(file_type == IL_TYPE_UNKNOWN)
		file_type = ilDetermineTypeL(data, size);

	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	success = ilLoadL(file_type, data, size);

	if (!success)
	{
		error = ilGetError();
		printf("Failed to load image: %s\n", iluErrorString(error));
		return 0;
	}

	r_tex = LoadGLTexture_IL_Image(imageID, channels, w, h, transparent, alpha_channel);

	ilDeleteImages(1, &imageID);

	return r_tex;
}

GLuint LoadGLTexture_IL_Image(ILuint imageID, int channels, int *w, int *h, bool *transparent, int alpha_channel)
{
	GLuint gl_tex;
	ILboolean success;
	ILinfo ImageInfo;
	ILint width, height;
	ILubyte *data;
	ILenum error;
	int i;

	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	ILenum il_format;

	switch(channels)
	{
		case 1:
			il_format = IL_LUMINANCE;
			break;
		case 2:
		case 3:
			il_format = IL_RGB;
			break;
		case 4:
		default:
			il_format = IL_RGBA;
			break;
	}

	success = ilConvertImage(il_format, IL_UNSIGNED_BYTE);

	if (!success)
	{
		error = ilGetError();
		printf("Failed to convert image: %s\n", iluErrorString(error));
		return 0;
	}

	glGenTextures(1, &gl_tex);

	glBindTexture(GL_TEXTURE_2D, gl_tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	data = ilGetData();

	if(w != 0)
		*w = width;
	if(h != 0)
		*h = height;

	if(transparent != 0 && channels == 4)
	{
		*transparent = false;
		for(i=0; i<width*height; i++)
		{
			if(data[i*4 + alpha_channel] < 128)
			{
				*transparent = true;
				break;
			}
		}
	}


	GLenum gl_format = (GLenum)ilGetInteger(IL_IMAGE_FORMAT);
	if(il_format == IL_LUMINANCE)
		gl_format = GL_RED;

	glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
				 0,								// Pyramid level (for mip-mapping) - 0 is the top level
				 gl_format,						// Image colour depth
				 width,							// Image width
				 height,						// Image height
				 0,								// Border width in pixels (can either be 1 or 0)
				 gl_format,	// Image format (i.e. RGB, RGBA, BGR etc.)
				 GL_UNSIGNED_BYTE,				// Image data type
				 data);							// The actual image data itself

	glGenerateMipmap(GL_TEXTURE_2D);

	return gl_tex;
}


GLuint LoadGLTextureArray_IL(const char **filenames, const int count, int *w, int *h)
{
	if(count <= 0)
		return 0;

	int width, height;

	ILuint *il_images = new ILuint[count];
	ilGenImages(count, il_images);

	for(int i=0; i<count; i++)
	{
		ILinfo image_info;

		ilBindImage(il_images[i]);

		if(!ilLoadImage(filenames[i]))
		{
			printf("Failed to load image \"%s\": %s\n", filenames[i], iluErrorString(ilGetError()));
			ilDeleteImages(count, il_images);
			delete[] il_images;
			return 0;
		}

		iluGetImageInfo(&image_info);

		if(i == 0)
		{
			width = image_info.Width;
			height = image_info.Height;
		}
		else if((int)image_info.Width != width || (int)image_info.Height != height)
		{
			printf("Size of image %s does not match others in texture array.\n", filenames[i]);
			ilDeleteImages(count, il_images);
			delete[] il_images;
			return 0;
		}

		if(image_info.Origin == IL_ORIGIN_UPPER_LEFT)
			iluFlipImage();

		if(!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
		{
			printf("Failed to convert image: %s\n", iluErrorString(ilGetError()));
			ilDeleteImages(count, il_images);
			delete[] il_images;
			return 0;
		}
	}

	GLuint gl_texture;
	glGenTextures(1, &gl_texture);

	glBindTexture(GL_TEXTURE_2D_ARRAY, gl_texture);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, count, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	for(int i=0; i<count; i++)
	{
		ilBindImage(il_images[i]);
		ILubyte *data = ilGetData();

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	if(w != 0)
		*w = width;
	if(h != 0)
		*h = height;

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	ilDeleteImages(count, il_images);
	delete[] il_images;

	return gl_texture;
}








GLuint LoadGLCubeMap_IL(const char *filename[6])
{
	ILinfo ImageInfo;
	ILboolean success;
	ILenum error;
	GLuint handle;
	int i;

	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, handle);

	//float max_anisotropy;

	for(i=0; i<6; i++)
	{
		success = ilLoadImage(filename[i]);
		if(!success)
		{
			printf("Failed to load cube map texture %d: %s\n", i, filename[i]);
			printf("%s\n", iluErrorString(ilGetError()));
			continue;
		}
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin != IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

		if (!success)
		{
			error = ilGetError();
			std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
			continue;
		}

		glTexImage2D(CubeTex(i), 				// Type of texture
					 0,				// Pyramid level (for mip-mapping) - 0 is the top level
					 ilGetInteger(IL_IMAGE_FORMAT),	// Image colour depth
					 ilGetInteger(IL_IMAGE_WIDTH),	// Image width
					 ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
					 0,				// Border width in pixels (can either be 1 or 0)
					 ilGetInteger(IL_IMAGE_FORMAT),	// Image format (i.e. RGB, RGBA, BGR etc.)
					 GL_UNSIGNED_BYTE,		// Image data type
					 ilGetData());			// The actual image data itself
		//gluBuild2DMipmaps(CubeTex(i), ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
		//			ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return handle;
}


GLuint LoadGLCubeMap_IL(const char *filename)
{
	ILuint image;
	ILboolean success;
	GLuint r;

	image = ilGenImage();
	ilBindImage(image);

	success = ilLoadImage(filename);
	if(!success)
	{
		printf("Failed to load cube map texture: %s\n", filename);
		printf("%s\n", iluErrorString(ilGetError()));
		return 0;
	}

	r = LoadGLCubeMap_IL_Image(image);
	ilDeleteImage(image);
	return r;
}

GLuint LoadGLCubeMapBinary_IL(const char *ext, const void *data, unsigned int size)
{
	ILuint imageID;
	ILboolean success;
	ILenum error;
	GLuint r_tex;

	ILenum file_type = IL_TYPE_UNKNOWN;

	if(ext)
		file_type = ilTypeFromExt((string("f.") + string(ext)).c_str());

	if(file_type == IL_TYPE_UNKNOWN)
		file_type = ilDetermineTypeL(data, size);

	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	success = ilLoadL(file_type, data, size);

	if (!success)
	{
		error = ilGetError();
		printf("Failed to load cube map texture: %s\n", iluErrorString(error));
		return 0;
	}

	r_tex = LoadGLCubeMap_IL_Image(imageID);

	ilDeleteImages(1, &imageID);

	return r_tex;
}

GLuint LoadGLCubeMap_IL_Image(ILuint image)
{
	ILinfo ImageInfo;
	ILboolean success;
	ILenum error;
	GLuint handle;

	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, handle);

	ilBindImage(image);

	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin != IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}
	success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

	if (!success)
	{
		error = ilGetError();
		std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
		return 0;
	}

	/*
	 * ----------------
	 * | -x | -z | +x |
	 * ----------------
	 * | -y | +y | +z |
	 * ----------------
	 */

	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);
	int side_width = width / 3;
	int side_height = height / 2;
	GLenum format = ilGetInteger(IL_IMAGE_FORMAT);
	GLint bpp = ilGetInteger(IL_IMAGE_BPP);
	GLubyte *data = ilGetData();

	GLubyte *side_data = new GLubyte[side_width * side_height * bpp];


	GetSubImage(side_data, data, width, height, bpp, 0, 0, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	GetSubImage(side_data, data, width, height, bpp, side_width, 0, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	GetSubImage(side_data, data, width, height, bpp, side_width * 2, 0, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);


	GetSubImage(side_data, data, width, height, bpp, 0, side_height, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	GetSubImage(side_data, data, width, height, bpp, side_width, side_height, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	GetSubImage(side_data, data, width, height, bpp, side_width * 2, side_height, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	delete [] side_data;

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);



	return handle;
}


#endif