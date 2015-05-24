
#include "towerengine.h"

using namespace std;

void tShader::SetSource(const char *vertex, const char *fragment)
{
	vertex_src = vertex;
	fragment_src = fragment;
}

void tShader::CreateVertexShader(void)
{
	vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_src, name);
}

void tShader::CreateFragmentShader(void)
{
	fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_src, name);
}

void tShader::CreateProgram(void)
{
	program = CreateShaderProgram(vertex_shader, fragment_shader);
}

void tShader::LinkProgram(void)
{
	LinkShaderProgram(program);
}

void tShader::InitShader(const char *vert_src, const char *frag_src, const char *shader_name)
{
	name = shader_name;

	SetSource(vert_src, frag_src);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();
}

GLint tShader::GetUniformLocation(const char *uniform)
{
	return glGetUniformLocation(program, uniform);
}

void tShader::Bind(void)
{
	glUseProgram(program);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	glShaderSource(s, 1, &src, &len);

	glCompileShader(s);
	PrintGLInfoLog("Compile", s, name);

	return s;
}

GLuint CreateShaderProgram(GLuint vertex_shader, GLuint fragment_shader)
{
	GLuint p;

	p = glCreateProgram();
	glAttachShader(p, vertex_shader);
	glAttachShader(p, fragment_shader);

	return p;
}

void LinkShaderProgram(GLuint program)
{
	glLinkProgram(program);
	//PrintGLInfoLog("Link", program);
}

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
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

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
				 ilGetInteger(IL_IMAGE_BPP),	// Image colour depth
				 width,							// Image width
				 height,						// Image height
				 0,								// Border width in pixels (can either be 1 or 0)
				 ilGetInteger(IL_IMAGE_FORMAT),	// Image format (i.e. RGB, RGBA, BGR etc.)
				 GL_UNSIGNED_BYTE,				// Image data type
				 data);							// The actual image data itself


	return textureID;
}

GLuint GLTextureFromColor(const tVector &color)
{
	const int width = 64;
	const int height = 64;
	unsigned char data[width * height * 3];
	unsigned char *p;
	GLuint handle;
	int i;

	unsigned char r = (unsigned char)fmax(0.0, fmin(255.0, floor(color.x * 256.0)));
	unsigned char g = (unsigned char)fmax(0.0, fmin(255.0, floor(color.y * 256.0)));
	unsigned char b = (unsigned char)fmax(0.0, fmin(255.0, floor(color.z * 256.0)));

	p = data;
	for(i = 0; i < width * height; i++)
	{
		*p++ = r;
		*p++ = g;
		*p++ = b;
	}

	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
	//printf("Color (%d, %d, %d) successfully loaded [handle = %d, width = %d, height = %d] :)\n", r, g, b, handle, width, height);

	return handle;
}


