
#include "towerengine.h"
#include "resources.h"

void tParticleShader::InitParticleShader(tGBuffer *gbuffer, const char *vertex_source, const char *geometry_source, const char *fragment_source)
{
	InitShader(vertex_source, fragment_source, "Particle Shader");
	CreateAndAttachShader(GL_GEOMETRY_SHADER, geometry_source);
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



void tParticleForwardShader::Init(tGBuffer *gbuffer)
{
	InitParticleShader(gbuffer, resources_get("particle_forward_shader.vert"), resources_get("particle_forward_shader.geom"), resources_get("particle_forward_shader.frag"));
}



void tParticleDeferredShader::Init(tGBuffer *gbuffer)
{
	InitParticleShader(gbuffer, resources_get("particle_deferred_shader.vert"), resources_get("particle_deferred_shader.geom"), resources_get("particle_deferred_shader.frag"));

	face_normal_uniform = GetUniformLocation("face_normal_uni");
	lighting_normal_uniform = GetUniformLocation("lighting_normal_uni");
}

void tParticleDeferredShader::SetFaceNormal(tVector normal)
{
	glUniform3f(face_normal_uniform, normal.x, normal.y, normal.z);
}

void tParticleDeferredShader::SetLightingNormal(tVector normal)
{
	glUniform3f(lighting_normal_uniform, normal.x, normal.y, normal.z);
}