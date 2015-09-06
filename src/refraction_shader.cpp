
#include "towerengine.h"
#include "tresources.h"

void tRefractionShader::Init(void)
{
	InitShader(refraction_shader_vert, refraction_shader_frag, "Refraction Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, uvcoord_attribute, "uv_attr");

	LinkProgram();

	modelview_projection_matrix_uniform = GetUniformLocation("modelview_projection_matrix_uni");

	color_uniform = GetUniformLocation("color_uni");

	color_tex_uniform = GetUniformLocation("color_tex_uni");
	color_tex_enabled_uniform = GetUniformLocation("color_tex_enabled_uni");

	normal_tex_uniform = GetUniformLocation("normal_tex_uni");

	screen_tex_uniform = GetUniformLocation("screen_tex_uni");

	transformation_uniform = GetUniformLocation("transformation_uni");

	Bind();
	glUniform1i(color_tex_uniform, diffuse_tex_unit);
	glUniform1i(screen_tex_uniform, screen_tex_unit);
	glUniform1i(normal_tex_uniform, normal_tex_unit);
}


void tRefractionShader::SetModelViewProjectionMatrix(float m[16])
{
	glUniformMatrix4fv(modelview_projection_matrix_uniform, 1, GL_TRUE, m);
}


void tRefractionShader::SetTransformation(const float m[16])
{
	glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}

void tRefractionShader::SetColor(tVector color)
{
	glUniform3f(color_uniform, color.x, color.y, color.z);
}


void tRefractionShader::SetColorTexture(bool enabled, GLuint tex)
{
	glUniform1i(color_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glActiveTexture(GL_TEXTURE0 + diffuse_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}

void tRefractionShader::SetNormalTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + normal_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void tRefractionShader::SetScreenTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + screen_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}
