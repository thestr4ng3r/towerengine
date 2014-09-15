
#include "towerengine.h"

void CDirectionalShadowShader::Init(void)
{
	SetSource(directional_shadow_shader_vert, directional_shadow_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();
	glBindAttribLocation(program, CFaceShader::vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, CFaceShader::vertex2_attribute, "vertex2_attr");
	LinkProgram();

	light_dir_uniform = GetUniformLocation("light_dir_uni");
	clip_uniform = GetUniformLocation("clip_uni");
	transformation_uniform = GetUniformLocation("transformation_uni");
	vertex_mix_uniform = GetUniformLocation("vertex_mix_uni");
	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	Unbind();
}

void CDirectionalShadowShader::SetLightDir(CVector v)
{
	glUniform3f(light_dir_uniform, v.x, v.y, v.z);
}

void CDirectionalShadowShader::SetTransformation(const float m[16])
{
	if(transformation_uniform != -1)
		glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
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





void CDirectionalShadowBlurShader::Init(void)
{
	InitShader(directional_shadow_blur_shader_vert, directional_shadow_blur_shader_frag, "Directional Shadow Blur Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, uv_coord_attribute, "uv_coord_attr");

	LinkProgram();

	tex_uniform = GetUniformLocation("tex_uni");
	tex_layers_count_uniform = GetUniformLocation("tex_layers_count_uni");
	blur_dir_uniform = GetUniformLocation("blur_dir_uni");
	blur_factors_uniform = GetUniformLocation("blur_factors_uni");

	Unbind();
}

void CDirectionalShadowBlurShader::SetTexture(GLuint tex)
{
	glUniform1i(tex_uniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
}

void CDirectionalShadowBlurShader::SetTextureLayers(int count, float *blur_factors)
{
	glUniform1i(tex_layers_count_uniform, count);
	glUniform1fv(blur_factors_uniform, count, blur_factors);
}


void CDirectionalShadowBlurShader::SetBlurDir(CVector2 v)
{
	glUniform2f(blur_dir_uniform, v.x, v.y);
}

