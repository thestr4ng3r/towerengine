
#include "towerengine.h"
#include "resources.h"

void tCubeMapBlurShader::Init(void)
{
	InitShader(get_resource("cube_map_blur_shader.vert"), get_resource("cube_map_blur_shader.frag"), "Cube Map Blur Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");

	LinkProgram();


	tex_uniform = GetUniformLocation("tex_uni");

	mipmap_level_uniform = GetUniformLocation("mipmap_level_uni");
	blur_dir_uniform = GetUniformLocation("blur_dir_uni");

	Bind();

	glUniform1i(tex_uniform, 0);
}

void tCubeMapBlurShader::SetTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
}

void tCubeMapBlurShader::SetBlurDir(tVector v)
{
	glUniform3f(blur_dir_uniform, v.x, v.y, v.z);
}

void tCubeMapBlurShader::SetMipmapLevel(int level)
{
	glUniform1i(mipmap_level_uniform, level);
}