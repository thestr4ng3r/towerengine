
#include "towerengine.h"
#include "tresources.h"

void tParticleShader::Init(void)
{
	InitShader(particle_shader_vert, particle_shader_frag, "Particle Shader");
	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	LinkProgram();

	modelview_projection_matrix_uniform = GetUniformLocation("modelview_projection_matrix_uni");

	color_uniform = GetUniformLocation("color_uni");
}


void tParticleShader::SetModelViewProjectionMatrix(float m[16])
{
	glUniformMatrix4fv(modelview_projection_matrix_uniform, 1, GL_TRUE, m);
}

void tParticleShader::SetColor(tVector color, float alpha)
{
	glUniform4f(color_uniform, color.x, color.y, color.z, alpha);
}
