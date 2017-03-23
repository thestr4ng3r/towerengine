
#include "towerengine.h"
#include "resources.h"

tStandardForwardShader::tStandardForwardShader(void)
{
	InitShader(resources_get("standard_forward_shader.vert"), resources_get("standard_forward_shader.frag"), "Standard Forward Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, uvcoord_attribute, "uv_attr");

	LinkProgram();

	transformation_uniform = GetUniformLocation("transformation_uni");

	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "MatrixBlock"), matrix_binding_point);

	Bind();
	glUniform1i(tex_uniform, base_color_tex_unit);
}



void tStandardForwardShader::SetTransformation(const float m[16])
{
	glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}
