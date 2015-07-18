
#include "towerengine.h"
#include "tresources.h"

void tParticleShader::Init(tGBuffer *gbuffer)
{
	InitShader(particle_shader_vert, particle_shader_frag, "Particle Shader");
	CreateAndAttachShader(GL_GEOMETRY_SHADER, particle_shader_geom);
	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, size_attribute, "size_attr");
	glBindAttribLocation(program, rotation_attribute, "rotation_attr");
	glBindAttribLocation(program, color_attribute, "color_attr");
	glBindAttribLocation(program, texture_index_attribute, "texture_index_attr");
	LinkProgram();

	modelview_matrix_uniform = GetUniformLocation("modelview_matrix_uni");
	projection_matrix_uniform = GetUniformLocation("projection_matrix_uni");

	texture_uniform = GetUniformLocation("texture_uni");

	Bind();
	texture_unit = gbuffer->GetLastTextureUnit()+1;
	glUniform1i(texture_uniform, texture_unit);
}


void tParticleShader::SetModelViewProjectionMatrices(float modelview[16], float projection[16])
{
	glUniformMatrix4fv(modelview_matrix_uniform, 1, GL_TRUE, modelview);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_TRUE, projection);
}

void tParticleShader::SetTexture(GLuint texture)
{
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
}
