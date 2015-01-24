
#include "towerengine.h"

void tDirectionalShadowShader::Init(void)
{
	SetSource(directional_shadow_shader_vert, directional_shadow_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();
	glBindAttribLocation(program, tFaceShader::vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, tFaceShader::vertex2_attribute, "vertex2_attr");
	glBindAttribLocation(program, tFaceShader::uvcoord_attribute, "uv_attr");
	LinkProgram();

	light_dir_uniform = GetUniformLocation("light_dir_uni");
	clip_uniform = GetUniformLocation("clip_uni");
	transformation_uniform = GetUniformLocation("transformation_uni");
	vertex_mix_uniform = GetUniformLocation("vertex_mix_uni");
	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	diffuse_tex_enabled_uniform = GetUniformLocation("diffuse_tex_enabled_uni");
	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");

	Unbind();
}

void tDirectionalShadowShader::SetLightDir(tVector v)
{
	glUniform3f(light_dir_uniform, v.x, v.y, v.z);
}

void tDirectionalShadowShader::SetTransformation(const float m[16])
{
	if(transformation_uniform != -1)
		glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}

void tDirectionalShadowShader::SetVertexMix(float m)
{
	glUniform1f(vertex_mix_uniform, m);
}

void tDirectionalShadowShader::SetClip(float near_clip, float far_clip)
{
	glUniform2f(clip_uniform, near_clip, far_clip);
}

void tDirectionalShadowShader::SetCamPos(tVector v)
{
	glUniform3f(cam_pos_uniform, v.x, v.y, v.z);
}



void tDirectionalShadowShader::SetDiffuseTexture(bool enabled, GLuint tex)
{
	glUniform1i(diffuse_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1i(diffuse_tex_uniform, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}




void tDirectionalShadowBlurShader::Init(void)
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

void tDirectionalShadowBlurShader::SetTexture(GLuint tex)
{
	glUniform1i(tex_uniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
}

void tDirectionalShadowBlurShader::SetTextureLayers(int count, float *blur_factors)
{
	glUniform1i(tex_layers_count_uniform, count);
	glUniform1fv(blur_factors_uniform, count, blur_factors);
}


void tDirectionalShadowBlurShader::SetBlurDir(tVector2 v)
{
	glUniform2f(blur_dir_uniform, v.x, v.y);
}

