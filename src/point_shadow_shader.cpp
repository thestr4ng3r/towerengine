
#include "towerengine.h"

void CPointShadowShader::Init(void)
{
	printf("Compiling Point Shadow Shader...\n");

	SetSource(point_shadow_shader_vert, point_shadow_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();
	glBindAttribLocationARB(program, CFaceShader::vertex_attribute, "vertex_attr");
	glBindAttribLocationARB(program, CFaceShader::vertex2_attribute, "vertex2_attr");
	LinkProgram();

	light_pos_uniform = glGetUniformLocationARB(program, "light_pos_uni");
	light_dist_uniform = glGetUniformLocationARB(program, "light_dist_uni");
	transformation_uniform = glGetUniformLocationARB(program, "transformation_uni");
	vertex_mix_uniform = glGetUniformLocationARB(program, "vertex_mix_uni");

	UseNoShader();
}

void CPointShadowShader::SetLightPos(CVector v)
{
	glUniform3f(light_pos_uniform, v.x, v.y, v.z);
}

void CPointShadowShader::SetLightDist(float d)
{
	glUniform1f(light_dist_uniform, d);
}

void CPointShadowShader::SetTransformation(const float m[16])
{
	if(transformation_uniform != -1)
		glUniformMatrix4fvARB(transformation_uniform, 1, GL_FALSE, m);
}

void CPointShadowShader::SetVertexMix(float m)
{
	glUniform1f(vertex_mix_uniform, m);
}






void CPointShadowBlurShader::Init(void)
{
	printf("Compiling Point Shadow Blur Shader...\n");
	SetSource(point_shadow_blur_shader_vert, point_shadow_blur_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();
	LinkProgram();

	tex_uniform = glGetUniformLocationARB(program, "tex_uni");
	blur_dir_uniform = glGetUniformLocationARB(program, "blur_dir_uni");

	UseNoShader();
}

void CPointShadowBlurShader::SetTexture(GLuint tex)
{
	glUniform1i(tex_uniform, 0);
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
}

void CPointShadowBlurShader::SetBlurDir(CVector v)
{
	glUniform3f(blur_dir_uniform, v.x, v.y, v.z);
}
