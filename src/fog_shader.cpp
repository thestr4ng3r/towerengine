
#include "towerengine.h"
#include "tresources.h"

void tFogShader::Init(void)
{
	InitScreenShader(fog_shader_frag, "Fog Shader");

	depth_tex_uniform = GetUniformLocation("depth_tex_uni");
	color_tex_uniform = GetUniformLocation("color_tex_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	start_dist_uniform = GetUniformLocation("start_dist_uni");
	end_dist_uniform = GetUniformLocation("end_dist_uni");
	exp_uniform = GetUniformLocation("exp_uni");
	color_uniform = GetUniformLocation("color_uni");

	Bind();
	glUniform1i(depth_tex_uniform, depth_tex_unit = 0);
	glUniform1i(color_tex_uniform, color_tex_unit = 1);
}

void tFogShader::SetTextures(GLuint position, GLuint color)
{
	glActiveTexture(GL_TEXTURE0 + depth_tex_unit);
	glBindTexture(GL_TEXTURE_2D, position);

	glActiveTexture(GL_TEXTURE1 + color_tex_unit);
	glBindTexture(GL_TEXTURE_2D, color);
}

void tFogShader::SetCameraPosition(tVector cam_pos)
{
	glUniform3f(cam_pos_uniform, cam_pos.x, cam_pos.y, cam_pos.z);
}

void tFogShader::SetFog(float start_dist, float end_dist, float exp, tVector color)
{
	glUniform1f(start_dist_uniform, start_dist);
	glUniform1f(end_dist_uniform, end_dist);
	glUniform1f(exp_uniform, exp);
	glUniform3f(color_uniform, color.x, color.y, color.z);
}
