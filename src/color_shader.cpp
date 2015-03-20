
#include "towerengine.h"
#include "tresources.h"


void tColorShader::Init(void)
{
	SetSource(color_shader_vert, color_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");

	LinkProgram();

	diffuse_color_uniform = GetUniformLocation("diffuse_color_uni");
	transformation_uniform = GetUniformLocation("transformation_uni");

	Bind();
	ResetUniforms();
	Unbind();
}

void tColorShader::SetTransformation(const float m[16])
{
	if(transformation_uniform != -1)
		glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}

void tColorShader::SetDiffuseColor(tVector color)
{
	if(diffuse_color_uniform != -1)
		glUniform3f(diffuse_color_uniform, color.x, color.y, color.z);
}

void tColorShader::ResetUniforms(void)
{
	SetDiffuseColor(Vec(1.0, 1.0, 1.0));
	SetTransformation(tEngine::identity_matrix4);
}
