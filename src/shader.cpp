
#include "towerengine.h"

#include <IL/il.h>
#include <IL/ilu.h>

//#define CODEXL_WORKAROUND


using namespace std;


void tShader::CreateAndAttachShader(GLenum type, const char *src)
{
	GLuint shader = CreateShader(type, src, name);
	glAttachShader(program, shader);
}

void tShader::LinkProgram(void)
{
	glLinkProgram(program);

	GLint param;
	glGetProgramiv(program, GL_LINK_STATUS, &param);
	if(param != GL_TRUE)
		printf("failed to link %s\n", name);
}

void tShader::InitShader(const char *vert_src, const char *frag_src, const char *shader_name)
{
	name = shader_name;
	program = glCreateProgram();
#ifdef GL_PROGRAM
	if(name)
		glObjectLabel(GL_PROGRAM, program, strlen(name), name);
#endif
	CreateAndAttachShader(GL_VERTEX_SHADER, vert_src);
	CreateAndAttachShader(GL_FRAGMENT_SHADER, frag_src);
}

GLint tShader::GetUniformLocation(const char *uniform)
{
	return glGetUniformLocation(program, uniform);
}

void tShader::Bind(void)
{
	glUseProgram(program);
}

void tShader::Unbind(void)
{
	glUseProgram(0);
}

void PrintGLInfoLog(const char *log_title, GLuint handle, const char *shader_name = 0)
{
	GLchar *string;
	GLint size;

	size = 0;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &size);
	if(size > 1)
	{
		string = new GLchar [size];
		glGetShaderInfoLog(handle, size, &size, string);
		printf("%s info log", log_title);
		if(shader_name != 0)
			 printf(" for %s:\n", shader_name);
		printf(":\n");
		printf("-----------\n%s\n-----------\n", string);
		delete [] string;
	}
}


GLuint CreateShader(GLenum type, const char *src, const char *name)
{
	GLuint s;
	GLint len;
	
	s = glCreateShader(type);
	if(!s)
	{
		printf("Could not create shader!\n");
		return 0;
	}


	len = strlen(src);

#ifdef CODEXL_WORKAROUND
	glShaderSourceARB(s, 1, &src, &len);
	printf("WARNING: using glShaderSourceARB instead of glShaderSource as a workaround for CodeXL.\n");
#else
	glShaderSource(s, 1, &src, &len);
#endif


	glCompileShader(s);

	GLint compile_status;
	glGetShaderiv(s, GL_COMPILE_STATUS, &compile_status);
	if(compile_status != GL_TRUE)
		printf("failed to compile shader of %s\n", name);

	PrintGLInfoLog("Compile", s, name);

	return s;
}

/*GLuint CreateShaderProgram(GLuint vertex_shader, GLuint fragment_shader)
{
	GLuint p;

	p = glCreateProgram();
	glAttachShader(p, vertex_shader);
	glAttachShader(p, fragment_shader);

	return p;
}*/

/*void LinkShaderProgram(GLuint program)
{
	glLinkProgram(program);
	//PrintGLInfoLog("Link", program);
}*/

GLuint LoadGLTextureIL(ILuint image, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);

GLuint LoadGLTexture(const char *filename, int *w, int *h, bool *transparent, int alpha_channel)
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

	r_tex = LoadGLTextureIL(imageID, w, h, transparent, alpha_channel);

	ilDeleteImages(1, &imageID);

	return r_tex;
}

GLuint LoadGLTextureBinary(const char *ext, const void *data, unsigned int size, int *w, int *h, bool *transparent, int alpha_channel)
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

	r_tex = LoadGLTextureIL(imageID, w, h, transparent, alpha_channel);

	ilDeleteImages(1, &imageID);

	return r_tex;
}

GLuint LoadGLTextureIL(ILuint imageID, int *w, int *h, bool *transparent, int alpha_channel) // from http://r3dux.org/2010/11/single-call-opengl-texture-loader-in-devil/
{
	GLuint textureID;
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
	success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	if (!success)
	{
		error = ilGetError();
		printf("Failed to convert image: %s\n", iluErrorString(error));
		return 0;
	}

	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

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

	if(transparent != 0)
	{
		*transparent = false;
		for(i=0; i<width*height; i++)
		{
			if(data[i*4 + alpha_channel] < 255)
			{
				*transparent = true;
				break;
			}
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
				 0,								// Pyramid level (for mip-mapping) - 0 is the top level
				 ilGetInteger(IL_IMAGE_FORMAT),	// Image colour depth
				 width,							// Image width
				 height,						// Image height
				 0,								// Border width in pixels (can either be 1 or 0)
				 ilGetInteger(IL_IMAGE_FORMAT),	// Image format (i.e. RGB, RGBA, BGR etc.)
				 GL_UNSIGNED_BYTE,				// Image data type
				 data);							// The actual image data itself

	glGenerateMipmap(GL_TEXTURE_2D);

	return textureID;
}


GLuint LoadGLTextureArray(const char **filenames, const int count, int *w, int *h)
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


