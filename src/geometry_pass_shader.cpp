
#include "towerengine.h"
#include "tresources.h"

void tGeometryPassShader::Init(void)
{
	InitShader(geometry_pass_shader_vert, geometry_pass_shader_frag, "Geometry Pass Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, vertex2_attribute, "vertex2_attr");
	glBindAttribLocation(program, normal_attribute, "normal_attr");
	glBindAttribLocation(program, tang_attribute, "tang_attr");
	glBindAttribLocation(program, bitang_attribute, "bitang_attr");
	glBindAttribLocation(program, uvcoord_attribute, "uv_attr");

	LinkProgram();

	modelview_projection_matrix_uniform = GetUniformLocation("modelview_projection_matrix_uni");
	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	vertex_mix_uniform = GetUniformLocation("vertex_mix_uni");

	material_index_uniform = GetUniformLocation("material_index_uni");

	diffuse_color_uniform = GetUniformLocation("diffuse_color_uni");
	diffuse_color2_uniform = GetUniformLocation("diffuse_color2_uni");
	specular_color_uniform = GetUniformLocation("specular_color_uni");
	specular_size_uniform = GetUniformLocation("specular_size_uni");
	self_illumination_color_uniform = GetUniformLocation("self_illumination_color_uni");

	bump_depth_uniform = GetUniformLocation("bump_depth_uni");

	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	specular_tex_uniform = GetUniformLocation("specular_tex_uni");
	bump_tex_uniform = GetUniformLocation("bump_tex_uni");
	self_illumination_tex_uniform = GetUniformLocation("self_illumination_tex_uni");

	diffuse_tex_enabled_uniform = GetUniformLocation("diffuse_tex_enabled_uni");
	normal_tex_enabled_uniform = GetUniformLocation("normal_tex_enabled_uni");
	specular_tex_enabled_uniform = GetUniformLocation("specular_tex_enabled_uni");
	bump_tex_enabled_uniform = GetUniformLocation("bump_tex_enabled_uni");
	self_illumination_tex_enabled_uniform = GetUniformLocation("self_illumination_tex_enabled_uni");

	cube_map_reflection_enabled_uniform = GetUniformLocation("cube_map_reflection_enabled_uni");
	cube_map_reflection_color_uniform = GetUniformLocation("cube_map_reflection_color_uni");
	cube_map_reflection_tex_uniform = GetUniformLocation("cube_map_reflection_tex_uni");

	transformation_uniform = GetUniformLocation("transformation_uni");

	clip_uniform = GetUniformLocation("clip_vec_uni");
	clip_dist_uniform = GetUniformLocation("clib_dist_uni");


	Bind();
	glUniform1i(diffuse_tex_uniform, diffuse_tex_unit);
	glUniform1i(specular_tex_uniform, specular_tex_unit);
	glUniform1i(normal_tex_uniform, normal_tex_unit);
	glUniform1i(bump_tex_uniform, bump_tex_unit);
	glUniform1i(self_illumination_tex_uniform, self_illumination_tex_unit);
	glUniform1i(cube_map_reflection_tex_uniform, cube_map_reflection_tex_unit);
}


void tGeometryPassShader::SetModelViewProjectionMatrix(float m[16])
{
	glUniformMatrix4fv(modelview_projection_matrix_uniform, 1, GL_TRUE, m);
}

void tGeometryPassShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}

void tGeometryPassShader::SetTransformation(const float m[16])
{
	glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}

void tGeometryPassShader::SetDiffuseColor(tVector color)
{
	glUniform3f(diffuse_color_uniform, color.x, color.y, color.z);
}

void tGeometryPassShader::SetDiffuseColor2(tVector color, float alpha)
{
	glUniform4f(diffuse_color2_uniform, color.x, color.y, color.z, alpha);
}

void tGeometryPassShader::SetSpecularColor(tVector color)
{
	glUniform3f(specular_color_uniform, color.x, color.y, color.z);
}


void tGeometryPassShader::SetSpecular(float size)
{
	glUniform1f(specular_size_uniform, size);
}


void tGeometryPassShader::SetDiffuseTexture(bool enabled, GLuint tex)
{
	glUniform1i(diffuse_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glActiveTexture(GL_TEXTURE0 + diffuse_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}

void tGeometryPassShader::SetSpecularTexture(bool enabled, GLuint tex)
{
	glUniform1i(specular_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glActiveTexture(GL_TEXTURE0 + specular_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

}

void tGeometryPassShader::SetNormalTexture(bool enabled, GLuint tex)
{
	glUniform1i(normal_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glActiveTexture(GL_TEXTURE0 + normal_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

}

void tGeometryPassShader::SetBumpDepth(float depth)
{
	glUniform1f(bump_depth_uniform, depth);
}

void tGeometryPassShader::SetBumpTexture(bool enabled, GLuint tex)
{
	glUniform1i(bump_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glActiveTexture(GL_TEXTURE0 + bump_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}


void tGeometryPassShader::SetCubeMapReflectionEnabled(bool enabled)
{
	glUniform1i(cube_map_reflection_enabled_uniform, enabled ? 1 : 0);
}

void tGeometryPassShader::SetCubeMapReflectionColor(tVector color)
{
	glUniform3f(cube_map_reflection_color_uniform, color.x, color.y, color.z);
}

void tGeometryPassShader::SetCubeMapReflectionTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + cube_map_reflection_tex_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
}


void tGeometryPassShader::SetClip(tVector c, float d)
{
	glUniform3f(clip_uniform, c.x, c.y, c.z);
	glUniform1f(clip_dist_uniform, d);
}

void tGeometryPassShader::SetVertexMix(float m)
{
	glUniform1f(vertex_mix_uniform, m);
}

void tGeometryPassShader::SetSelfIlluminationColor(tVector color)
{
	glUniform3f(self_illumination_color_uniform, color.x, color.y, color.z);
}

void tGeometryPassShader::SetSelfIlluminationTexture(bool enabled, GLuint tex)
{
	glUniform1i(self_illumination_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glActiveTexture(GL_TEXTURE0 + self_illumination_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}

void tGeometryPassShader::SetMaterialIndex(int index)
{
	glUniform1ui(material_index_uniform, index);
}
