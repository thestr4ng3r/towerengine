
#include "towerengine.h"
#include "resources.h"

void tSimpleForwardShader::Init(void)
{
	InitShader(resources_get("simple_forward_shader.vert"), resources_get("simple_forward_shader.frag"), "Simple Forward Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, uvcoord_attribute, "uv_attr");

	LinkProgram();

	color_uniform = GetUniformLocation("color_uni");

	tex_uniform = GetUniformLocation("tex_uni");
	tex_enabled_uniform = GetUniformLocation("tex_enabled_uni");

	transformation_uniform = GetUniformLocation("transformation_uni");

	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "MatrixBlock"), matrix_binding_point);

	Bind();
	glUniform1i(tex_uniform, diffuse_tex_unit);
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
