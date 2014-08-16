#include "towerengine.h"

void CSkyBoxShader::Init(void)
{
	SetSource(cube_env_shader_vert, cube_env_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();
	LinkProgram();

	tex_uniform = GetUniformLocation("Texture");

	Unbind();
}

void CSkyBoxShader::SetCubeMap(GLuint tex)
{
	glUniform1iARB(tex_uniform, 0);

	glActiveTextureARB(GL_TEXTURE0_ARB);
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

CVector CubeVecS(int side)
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



GLuint CreateCubeTexture(GLenum type, int width, int height)
{
	GLuint tex;

	glGenTextures(1, &tex);
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(type, tex);
	glTexParameterf(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(type, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(type, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	return tex;
}

