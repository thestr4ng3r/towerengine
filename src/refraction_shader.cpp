
#include "towerengine.h"
#include "resources.h"

tRefractionShader::tRefractionShader(void)
{
	InitShader(resources_get("refraction_shader.vert"), resources_get("refraction_shader.frag"), "Refraction Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, normal_attribute, "normal_attr");
	glBindAttribLocation(program, tang_attribute, "tang_attr");
	glBindAttribLocation(program, bitang_attribute, "bitang_attr");
	glBindAttribLocation(program, uvcoord_attribute, "uv_attr");

	LinkProgram();

	color_uniform = GetUniformLocation("color_uni");
	edge_color_uniform = GetUniformLocation("edge_color_uni");

	color_tex_uniform = GetUniformLocation("color_tex_uni");
	color_tex_enabled_uniform = GetUniformLocation("color_tex_enabled_uni");

	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	normal_tex_enabled_uniform = GetUniformLocation("normal_tex_enabled_uni");

	screen_tex_uniform = GetUniformLocation("screen_tex_uni");

	transformation_uniform = GetUniformLocation("transformation_uni");
	cam_pos_uniform = GetUniformLocation("cam_pos_uni");


	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "MatrixBlock"), matrix_binding_point);


	Bind();
	glUniform1i(color_tex_uniform, base_color_tex_unit);
	glUniform1i(screen_tex_uniform, screen_tex_unit);
	glUniform1i(normal_tex_uniform, normal_tex_unit);
}


void tRefractionShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}

void tRefractionShader::SetTransformation(const float m[16])
{
	glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}

void tRefractionShader::SetColor(tVector color, tVector edge, float edge_alpha)
{
	glUniform3f(color_uniform, color.x, color.y, color.z);
	glUniform4f(edge_color_uniform, edge.x, edge.y, edge.z, edge_alpha);
}


void tRefractionShader::SetColorTexture(bool enabled, GLuint tex)
{
	glUniform1i(color_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glActiveTexture(GL_TEXTURE0 + base_color_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}

void tRefractionShader::SetNormalTexture(bool enabled, GLuint tex)
{
	glUniform1i(normal_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glActiveTexture(GL_TEXTURE0 + normal_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}

void tRefractionShader::SetScreenTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + screen_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}
