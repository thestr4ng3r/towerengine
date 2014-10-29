
#include "towerengine.h"

void tPointShadowShader::Init(void)
{
	SetSource(point_shadow_shader_vert, point_shadow_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();
	glBindAttribLocation(program, tFaceShader::vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, tFaceShader::vertex2_attribute, "vertex2_attr");
	LinkProgram();

	light_pos_uniform = GetUniformLocation("light_pos_uni");
	light_dist_uniform = GetUniformLocation("light_dist_uni");
	transformation_uniform = GetUniformLocation("transformation_uni");
	vertex_mix_uniform = GetUniformLocation("vertex_mix_uni");

	Unbind();
}

void tPointShadowShader::SetLightPos(tVector v)
{
	glUniform3f(light_pos_uniform, v.x, v.y, v.z);
}

void tPointShadowShader::SetLightDist(float d)
{
	glUniform1f(light_dist_uniform, d);
}

void tPointShadowShader::SetTransformation(const float m[16])
{
	if(transformation_uniform != -1)
		glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}

void tPointShadowShader::SetVertexMix(float m)
{
	glUniform1f(vertex_mix_uniform, m);
}






void tPointShadowBlurShader::Init(void)
{
	SetSource(point_shadow_blur_shader_vert, point_shadow_blur_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();
	LinkProgram();

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");

	tex_uniform = GetUniformLocation("tex_uni");
	blur_dir_uniform = GetUniformLocation("blur_dir_uni");

	Unbind();
}

void tPointShadowBlurShader::SetTexture(GLuint tex)
{
	glUniform1i(tex_uniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
}

void tPointShadowBlurShader::SetBlurDir(tVector v)
{
	glUniform3f(blur_dir_uniform, v.x, v.y, v.z);
}
