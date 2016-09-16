
#include "towerengine.h"
#include "resources.h"

tPointShadowShader::tPointShadowShader(void)
{
	InitShader(resources_get("point_shadow_shader.vert"), resources_get("point_shadow_shader.frag"), "Point Shadow Shader");
	glBindAttribLocation(program, tFaceShader::vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, tFaceShader::uvcoord_attribute, "uv_attr");
	LinkProgram();

	modelview_projection_matrix_uniform = GetUniformLocation("modelview_projection_matrix_uni");

	light_pos_uniform = GetUniformLocation("light_pos_uni");
	light_dist_uniform = GetUniformLocation("light_dist_uni");
	transformation_uniform = GetUniformLocation("transformation_uni");

	diffuse_tex_enabled_uniform = GetUniformLocation("diffuse_tex_enabled_uni");
	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");

	Bind();
	glUniform1i(diffuse_tex_uniform, 0);
}

void tPointShadowShader::SetModelViewProjectionMatrix(float m[16])
{
	glUniformMatrix4fv(modelview_projection_matrix_uniform, 1, GL_TRUE, m);
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

void tPointShadowShader::SetBaseColorTexture(bool enabled, GLuint tex)
{
	glUniform1i(diffuse_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}





tPointShadowBlurShader::tPointShadowBlurShader(void)
{
	InitShader(resources_get("point_shadow_blur_shader.vert"), resources_get("point_shadow_blur_shader.frag"), "Point Shadow Blur Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");

	LinkProgram();


	tex_uniform = GetUniformLocation("tex_uni");

	blur_dir_uniform = GetUniformLocation("blur_dir_uni");

	Bind();
	glUniform1i(tex_uniform, 0);
}

void tPointShadowBlurShader::SetTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
}

void tPointShadowBlurShader::SetBlurDir(tVector v)
{
	glUniform3f(blur_dir_uniform, v.x, v.y, v.z);
}

