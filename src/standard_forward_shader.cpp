
#include "towerengine.h"
#include "resources.h"
#include "shader_source.h"

tStandardForwardShader::tStandardForwardShader(void)
{
	tShaderSource src(resources_get("standard_forward_shader.frag"));
	src.SetParameter("max_point_lights_count", new tShaderSourceVariable(max_point_lights_count));
	InitShader(resources_get("standard_forward_shader.vert"), src.BuildSource().c_str(), "Standard Forward Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, normal_attribute, "normal_attr");
	glBindAttribLocation(program, tang_attribute, "tang_attr");
	glBindAttribLocation(program, bitang_attribute, "bitang_attr");
	glBindAttribLocation(program, uvcoord_attribute, "uv_attr");

	LinkProgram();

	base_color_tex_uniform = GetUniformLocation("base_color_tex_uni");
	metallic_roughness_tex_uniform = GetUniformLocation("metallic_roughness_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	bump_tex_uniform = GetUniformLocation("bump_tex_uni");
	emission_tex_uniform = GetUniformLocation("emission_tex_uni");

	transformation_uniform = GetUniformLocation("transformation_uni");



	light_ambient_color_uniform = GetUniformLocation("light_ambient_color_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	/*reflection_tex1_uniform = GetUniformLocation("reflection_tex1_uni");
	reflection_tex2_uniform = GetUniformLocation("reflection_tex2_uni");
	reflection_tex_blend_uniform = GetUniformLocation("reflection_tex_blend_uni");*/


	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "MatrixBlock"), matrix_binding_point);
	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "MaterialBlock"), material_binding_point);
	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "PointLightBlock"), point_light_binding_point);

	Bind();
	glUniform1i(base_color_tex_uniform, base_color_tex_unit);
	glUniform1i(metallic_roughness_tex_uniform, metallic_roughness_tex_unit);
	glUniform1i(normal_tex_uniform, normal_tex_unit);
	glUniform1i(bump_tex_uniform, bump_tex_unit);
	glUniform1i(emission_tex_uniform, emission_tex_unit);
}


void tStandardForwardShader::SetTransformation(const float m[16])
{
	glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}

void tStandardForwardShader::SetBaseColorTexture(bool enabled, GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + base_color_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void tStandardForwardShader::SetMetallicRoughnessTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + metallic_roughness_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void tStandardForwardShader::SetNormalTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + normal_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}


void tStandardForwardShader::SetBumpTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + bump_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void tStandardForwardShader::SetEmissionTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + emission_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}





void tStandardForwardShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}


void tStandardForwardShader::SetAmbientLight(tVector color)
{
	glUniform3f(light_ambient_color_uniform, color.x, color.y, color.z);
}




