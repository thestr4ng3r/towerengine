
#include "towerengine.h"

void CDirectionalShadowShader::Init(void)
{
	printf("Compiling Directional Shadow Shader...\n");

	SetSource(directional_shadow_shader_vert, directional_shadow_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();
	glBindAttribLocationARB(program, CFaceShader::vertex_attribute, "vertex_attr");
	glBindAttribLocationARB(program, CFaceShader::vertex2_attribute, "vertex2_attr");
	LinkProgram();

	light_dir_uniform = glGetUniformLocationARB(program, "light_dir_uni");
	clip_uniform = glGetUniformLocationARB(program, "clip_uni");
	transformation_uniform = glGetUniformLocationARB(program, "transformation_uni");
	vertex_mix_uniform = glGetUniformLocationARB(program, "vertex_mix_uni");
	cam_pos_uniform = glGetUniformLocationARB(program, "cam_pos_uni");

	UseNoShader();
}

void CDirectionalShadowShader::SetLightDir(CVector v)
{
	glUniform3f(light_dir_uniform, v.x, v.y, v.z);
}

void CDirectionalShadowShader::SetTransformation(const float m[16])
{
	if(transformation_uniform != -1)
		glUniformMatrix4fvARB(transformation_uniform, 1, GL_FALSE, m);
}

void CDirectionalShadowShader::SetVertexMix(float m)
{
	glUniform1f(vertex_mix_uniform, m);
}

void CDirectionalShadowShader::SetClip(float near, float far)
{
	glUniform2f(clip_uniform, near, far);
}

void CDirectionalShadowShader::SetCamPos(CVector v)
{
	glUniform3f(cam_pos_uniform, v.x, v.y, v.z);
}
