
#include "towerengine.h"
#include "resources.h"


tColorShader::tColorShader(void)
{
	InitShader(resources_get("color_shader.vert"), resources_get("color_shader.frag"));

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, color_attribute, "color_attr");

	LinkProgram();

	modelview_projection_matrix_uniform = GetUniformLocation("modelview_projection_matrix_uni");
	transformation_uniform = GetUniformLocation("transformation_uni");
}

void tColorShader::SetModelViewProjectionmatrix(const float m[16])
{
	glUniformMatrix4fv(modelview_projection_matrix_uniform, 1, GL_TRUE, m);
}

void tColorShader::SetTransformation(const float m[16])
{
	glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}
