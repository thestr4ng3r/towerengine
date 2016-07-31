
#include "towerengine.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include "resources.h"


using namespace std;

void PrintGLInfoLog(const char *log_title, GLuint handle, const char *shader_name = 0);


void tShader::InitSource(void)
{
	const char **sources;
	const char **names;
	unsigned int source_count = resources_get_all(&names, &sources);

	for(unsigned int i=0; i<source_count; i++)
	{
		string name = string("/") + names[i];
		glNamedStringARB(GL_SHADER_INCLUDE_ARB, -1, name.c_str(), -1, sources[i]);
	}
}



GLuint tShader::CreateShaderFromSource(GLenum type, const char *src)
{
	GLuint shader = glCreateShader(type);

	GLint len = (GLint)strlen(src);
	glShaderSource(shader, 1, &src, &len);

	return shader;
}


GLuint tShader::CreateShaderFromNamedString(GLenum type, const char *name)
{
	int len;
	glGetNamedStringivARB(-1, name, GL_NAMED_STRING_LENGTH_ARB, &len);

	if(len <= 0)
		return 0;

	GLint stringlen;

	GLchar *source = new char[len];
	glGetNamedStringARB(-1, name, len, &stringlen, source);

	GLuint shader = CreateShaderFromSource(type, source);

	delete [] source;

	return shader;
}

void tShader::CompileAndAttachShader(GLuint shader)
{
	//glCompileShader(shader);

	const GLchar *search_path = "/";
	const GLint search_path_len[] = { -1 };
	glCompileShaderIncludeARB(shader, 1, &search_path, search_path_len);

	GLint compile_status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if(compile_status != GL_TRUE)
		printf("failed to compile shader of %s\n", name);

	PrintGLInfoLog("Compile", shader, name);

	glAttachShader(program, shader);
}

void tShader::LinkProgram(void)
{
	status = false;
	glLinkProgram(program);

	GLint param;
	glGetProgramiv(program, GL_LINK_STATUS, &param);
	if(param != GL_TRUE)
		printf("failed to link %s\n", name);

	status = param == GL_TRUE;
}

void tShader::InitShader(const char *vert_src, const char *frag_src, const char *shader_name)
{
	status = false;
	name = shader_name;
	program = glCreateProgram();
#ifdef TOWERENGINE_SHADER_LABELS
	if(name)
		glObjectLabel(GL_PROGRAM, program, strlen(name), name);
#endif
	CompileAndAttachShader(CreateShaderFromSource(GL_VERTEX_SHADER, vert_src));
	CompileAndAttachShader(CreateShaderFromSource(GL_FRAGMENT_SHADER, frag_src));
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


// ------------------------



tUniformBuffer::tUniformBuffer(unsigned int size)
{
	this->size = size;
	data = new unsigned char[this->size];

	glGenBuffers(1, &buffer);
}

tUniformBuffer::~tUniformBuffer()
{
	glDeleteBuffers(1, &buffer);
	delete [] data;
}

void tUniformBuffer::UploadData(GLenum usage)
{
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	glBufferData(GL_UNIFORM_BUFFER, size, data, usage);
}

void tUniformBuffer::Bind(GLuint binding_point)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, buffer);
}



// ------------------------



tMatrixBuffer::tMatrixBuffer(void)
		: tUniformBuffer(4*16)
{
}

tMatrixBuffer::~tMatrixBuffer(void)
{
}



void tMatrixBuffer::UpdateBuffer(const tMatrix4 &matrix)
{
	matrix.GetDataTranspose((float *)data);
	UploadData();
}

void tMatrixBuffer::Bind(void)
{
	tUniformBuffer::Bind(tShader::matrix_binding_point);
}




// ------------------------

void PrintGLInfoLog(const char *log_title, GLuint handle, const char *shader_name)
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



GLuint LoadGLTextureIL(ILuint image, int channels, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);

GLuint LoadGLTexture(const char *filename, int channels, int *w, int *h, bool *transparent, int alpha_channel)
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

	r_tex = LoadGLTextureIL(imageID, channels, w, h, transparent, alpha_channel);

	ilDeleteImages(1, &imageID);

	return r_tex;
}

GLuint LoadGLTextureBinary(const char *ext, const void *data, size_t size, int channels, int *w, int *h, bool *transparent, int alpha_channel)
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

	r_tex = LoadGLTextureIL(imageID, channels, w, h, transparent, alpha_channel);

	ilDeleteImages(1, &imageID);

	return r_tex;
}

GLuint LoadGLTextureIL(ILuint imageID, int channels, int *w, int *h, bool *transparent, int alpha_channel)
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


