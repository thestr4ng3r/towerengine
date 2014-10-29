
#include "towerengine.h"


void CColorShader::Init(void)
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

void CColorShader::SetTransformation(const float m[16])
{
	if(transformation_uniform != -1)
		glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}

void CColorShader::SetDiffuseColor(CVector color)
{
	if(diffuse_color_uniform != -1)
		glUniform3f(diffuse_color_uniform, color.x, color.y, color.z);
}

void CColorShader::ResetUniforms(void)
{
	SetDiffuseColor(Vec(1.0, 1.0, 1.0));
	SetTransformation(CEngine::identity_matrix4);
}
