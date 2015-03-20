#include "towerengine.h"
#include "tresources.h"

void tSkyBoxShader::Init(void)
{
	InitShader(cube_env_shader_vert, cube_env_shader_frag, "SkyBox Shader");
	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	LinkProgram();

	tex_uniform = GetUniformLocation("cube_map_uni");

	Unbind();
}

void tSkyBoxShader::SetCubeMap(GLuint tex)
{
	glUniform1i(tex_uniform, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
}

GLenum CubeTex(int side)
{
	switch(side)
	{
		case 0:	return GL_TEXTURE_CUBE_MAP_POSITIVE_X; break;
		case 1:	return GL_TEXTURE_CUBE_MAP_NEGATIVE_X; break;
		case 2:	return GL_TEXTURE_CUBE_MAP_POSITIVE_Y; break;
		case 3:	return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y; break;
		case 4:	return GL_TEXTURE_CUBE_MAP_POSITIVE_Z; break;
		case 5:	return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; break;
	};

	return 0;
}

int CubeSide(GLenum tex)
{
	switch(tex)
	{
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:	return 0; break;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:	return 1; break;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:	return 2; break;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:	return 3; break;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:	return 4; break;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:	return 5; break;
	};

	return 0;
}

tVector CubeVecS(int side)
{
	switch(side)
	{
		case 0:	return Vec(1.0, 0.0, 0.0); break;
		case 1:	return Vec(-1.0, 0.0, 0.0); break;
		case 2:	return Vec(0.0, 1.0, 0.0); break;
		case 3:	return Vec(0.0, -1.0, 0.0); break;
		case 4:	return Vec(0.0, 0.0, 1.0); break;
		case 5:	return Vec(0.0, 0.0, -1.0); break;
	};

	return Vec(0.0, 0.0, 0.0);
}

GLuint LoadGLCubeMap(const char *filename[6])
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
					 ilGetInteger(IL_IMAGE_BPP),	// Image colour depth
					 ilGetInteger(IL_IMAGE_WIDTH),	// Image width
					 ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
					 0,				// Border width in pixels (can either be 1 or 0)
					 ilGetInteger(IL_IMAGE_FORMAT),	// Image format (i.e. RGB, RGBA, BGR etc.)
					 GL_UNSIGNED_BYTE,		// Image data type
					 ilGetData());			// The actual image data itself
		gluBuild2DMipmaps(CubeTex(i), ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
					ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return handle;
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

GLuint LoadGLCubeMap(const char *filename)
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

	r = LoadGLCubeMapIL(image);
	ilDeleteImage(image);
	return r;
}

GLuint LoadGLCubeMapBinary(const char *ext, const void *data, unsigned int size)
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

	r_tex = LoadGLCubeMapIL(imageID);

	ilDeleteImages(1, &imageID);

	return r_tex;
}

GLuint LoadGLCubeMapIL(ILuint image)
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
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, bpp, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	GetSubImage(side_data, data, width, height, bpp, side_width, 0, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, bpp, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	GetSubImage(side_data, data, width, height, bpp, side_width * 2, 0, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, bpp, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);


	GetSubImage(side_data, data, width, height, bpp, 0, side_height, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, bpp, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	GetSubImage(side_data, data, width, height, bpp, side_width, side_height, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, bpp, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	GetSubImage(side_data, data, width, height, bpp, side_width * 2, side_height, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, bpp, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	delete [] side_data;

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);



	return handle;
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
	glTexParameteri(type, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(type, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	return tex;
}

