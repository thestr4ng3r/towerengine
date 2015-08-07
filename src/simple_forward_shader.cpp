
#include "towerengine.h"
#include "tresources.h"

void tSimpleForwardShader::Init(void)
{
	InitShader(simple_forward_shader_vert, simple_forward_shader_frag, "Simple Forward Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, uvcoord_attribute, "uv_attr");

	LinkProgram();

	modelview_projection_matrix_uniform = GetUniformLocation("modelview_projection_matrix_uni");

	color_uniform = GetUniformLocation("color_uni");

	tex_uniform = GetUniformLocation("tex_uni");
	tex_enabled_uniform = GetUniformLocation("tex_enabled_uni");

	transformation_uniform = GetUniformLocation("transformation_uni");

	Bind();
	glUniform1i(tex_uniform, diffuse_tex_unit);
}


void tSimpleForwardShader::SetModelViewProjectionMatrix(float m[16])
{
	glUniformMatrix4fv(modelview_projection_matrix_uniform, 1, GL_TRUE, m);
}


void tSimpleForwardShader::SetTransformation(const float m[16])
{
	glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}

void tSimpleForwardShader::SetColor(tVector color, float alpha)
{
	glUniform4f(color_uniform, color.x, color.y, color.z, alpha);
}


void tSimpleForwardShader::SetTexture(bool enabled, GLuint tex)
{
	glUniform1i(tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glActiveTexture(GL_TEXTURE0 + diffuse_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}
