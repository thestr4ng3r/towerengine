
#include "towerengine.h"
#include "tresources.h"

void tFogShader::Init(void)
{
	InitShader(fog_shader_vert, fog_shader_frag, "Post Process Shader");
	LinkProgram();

	position_tex_uniform = GetUniformLocation("position_tex_uni");
	color_tex_uniform = GetUniformLocation("color_tex_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	start_dist_uniform = GetUniformLocation("start_dist_uni");
	end_dist_uniform = GetUniformLocation("end_dist_uni");
	exp_uniform = GetUniformLocation("exp_uni");

	Bind();
	glUniform1i(position_tex_uniform, 0);
	glUniform1i(color_tex_uniform, 1);
}

void tFogShader::SetTextures(GLuint position, GLuint color)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, position);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, color);
}

void tFogShader::SetCameraPosition(tVector cam_pos)
{
	glUniform3f(cam_pos_uniform, cam_pos.x, cam_pos.y, cam_pos.z);
}

void tFogShader::SetFog(float start_dist, float end_dist, float exp)
{
	glUniform1f(start_dist_uniform, start_dist*start_dist);
	glUniform1f(end_dist_uniform, end_dist*end_dist);
	glUniform1f(exp_uniform, exp);
}
