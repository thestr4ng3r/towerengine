
#include "towerengine.h"
#include "resources.h"

void tGeometryPassShader::Init(void)
{
	InitShader(get_resource("geometry_pass_shader.vert"), get_resource("geometry_pass_shader.frag"), "Geometry Pass Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, normal_attribute, "normal_attr");
	glBindAttribLocation(program, tang_attribute, "tang_attr");
	glBindAttribLocation(program, bitang_attribute, "bitang_attr");
	glBindAttribLocation(program, uvcoord_attribute, "uv_attr");

	LinkProgram();

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	specular_tex_uniform = GetUniformLocation("specular_tex_uni");
	bump_tex_uniform = GetUniformLocation("bump_tex_uni");
	self_illumination_tex_uniform = GetUniformLocation("self_illumination_tex_uni");

	cube_map_reflection_tex_uniform = GetUniformLocation("cube_map_reflection_tex_uni");

	transformation_uniform = GetUniformLocation("transformation_uni");

	//clip_uniform = GetUniformLocation("clip_vec_uni");
	//clip_dist_uniform = GetUniformLocation("clib_dist_uni");

	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "MatrixBlock"), matrix_binding_point);
	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "MaterialBlock"), material_binding_point);

	Bind();
	glUniform1i(diffuse_tex_uniform, diffuse_tex_unit);
	glUniform1i(specular_tex_uniform, specular_tex_unit);
	glUniform1i(normal_tex_uniform, normal_tex_unit);
	glUniform1i(bump_tex_uniform, bump_tex_unit);
	glUniform1i(self_illumination_tex_uniform, self_illumination_tex_unit);
	glUniform1i(cube_map_reflection_tex_uniform, cube_map_reflection_tex_unit);
}


void tGeometryPassShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}

void tGeometryPassShader::SetTransformation(const float m[16])
{
	glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}

void tGeometryPassShader::SetDiffuseTexture(bool enabled, GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + diffuse_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void tGeometryPassShader::SetSpecularTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + specular_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void tGeometryPassShader::SetNormalTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + normal_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}


void tGeometryPassShader::SetBumpTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + bump_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}



void tGeometryPassShader::SetCubeMapReflectionTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + cube_map_reflection_tex_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
}



void tGeometryPassShader::SetSelfIlluminationTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + self_illumination_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}
