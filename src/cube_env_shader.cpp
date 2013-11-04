#include "towerengine.h"

CCubeEnvShader cube_env_shader;

void CCubeEnvShader::Init(void)
{
	printf("Compiling Environment Shader...\n");

	SetSource(cube_env_shader_vert, cube_env_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();
	LinkProgram();

	tex_uniform = glGetUniformLocationARB(program, "Texture");

	UseNoShader();
}

void CCubeEnvShader::SetCubeMap(GLuint tex)
{
	glUniform1iARB(tex_uniform, 0);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
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
			continue;
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

