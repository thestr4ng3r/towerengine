
#include "towerengine.h"
#include "tresources.h"

void tSSAOShader::Init(void)
{
	InitShader(ssao_shader_vert, ssao_shader_frag, "SSAO Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");

	LinkProgram();

	kernel_uniform = GetUniformLocation("kernel_uni");
	kernel_size_uniform = GetUniformLocation("kernel_size_uni");
	noise_tex_uniform = GetUniformLocation("noise_tex_uni");
	noise_tex_scale_uniform = GetUniformLocation("noise_tex_scale_uni");

	depth_tex_uniform = GetUniformLocation("depth_tex_uni");
	position_tex_uniform = GetUniformLocation("position_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");

	projection_matrix_uniform = GetUniformLocation("projection_matrix_uni");
	modelview_matrix_uniform = GetUniformLocation("modelview_matrix_uni");

	radius_uniform = GetUniformLocation("radius_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");
	cam_dir_uniform = GetUniformLocation("cam_dir_uni");
}

void tSSAOShader::SetKernel(int kernel_size, float *kernel)
{
	int s = min(kernel_size, MAX_SSAO_KERNEL_SIZE);

	glUniform1i(kernel_size_uniform, s);
	glUniform3fv(kernel_uniform, s, kernel);
}

void tSSAOShader::SetNoiseTex(GLuint tex, tVector2 tex_scale)
{
	glUniform2f(noise_tex_scale_uniform, tex_scale.x, tex_scale.y);

	glUniform1i(noise_tex_uniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void tSSAOShader::SetTextures(GLuint depth, GLuint pos, GLuint normal)
{
	glUniform1i(depth_tex_uniform, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depth);

	glUniform1i(position_tex_uniform, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, pos);

	glUniform1i(normal_tex_uniform, 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normal);
}

void tSSAOShader::SetMatrices(float *proj, float *modelview)
{
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, proj);
	glUniformMatrix4fv(modelview_matrix_uniform, 1, GL_FALSE, modelview);
}

void tSSAOShader::SetRadius(float radius)
{
	glUniform1f(radius_uniform, radius);
}

void tSSAOShader::SetCamera(tVector pos, tVector dir)
{
	glUniform3f(cam_dir_uniform, pos.x, pos.y, pos.z);
	glUniform3f(cam_pos_uniform, dir.x, dir.y, dir.z);
}
