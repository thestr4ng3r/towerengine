
#include "towerengine.h"

void CDirectionalShadowShader::Init(void)
{
	SetSource(directional_shadow_shader_vert, directional_shadow_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();
	glBindAttribLocationARB(program, CFaceShader::vertex_attribute, "vertex_attr");
	glBindAttribLocationARB(program, CFaceShader::vertex2_attribute, "vertex2_attr");
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





void CDirectionalShadowBlurShader::Init(void)
{
	InitShader(directional_shadow_blur_shader_vert, directional_shadow_blur_shader_frag, "Directional Shadow Shader");

	tex_uniform = GetUniformLocation("tex_uni");
	tex_layer_uniform = GetUniformLocation("tex_layer_uni");
	blur_dir_uniform = GetUniformLocation("blur_dir_uni");

	Unbind();
}

void CDirectionalShadowBlurShader::SetTexture(GLuint tex)
{
	glUniform1i(tex_uniform, 0);
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
}

void CDirectionalShadowBlurShader::SetTextureLayer(int layer)
{
	glUniform1f(tex_layer_uniform, (float)layer);
}


void CDirectionalShadowBlurShader::SetBlurDir(CVector2 v)
{
	glUniform2f(blur_dir_uniform, v.x, v.y);
}
