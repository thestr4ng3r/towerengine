
#include "towerengine.h"
#include "tresources.h"

void tCubeMapReflectionShader::Init(tGBuffer *gbuffer)
{
	InitShader(cube_map_reflection_shader_vert, cube_map_reflection_shader_frag, "CubeMap Reflection Shader");
	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	LinkProgram();

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	reflection_color_uniform = GetUniformLocation("reflection_color_uni");
	cube_map_uniform = GetUniformLocation("cube_map_uni");

	normal_tex_uniform = GetUniformLocation("normal_tex_uni");

	transformation_uniform = GetUniformLocation("transformation_uni");

	Bind();

	glUniform1i(normal_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::NORMAL_TEX));

	cube_map_tex_unit = gbuffer->GetLastTextureUnit() + 1;
	glUniform1i(cube_map_uniform, cube_map_tex_unit);
}

void tCubeMapReflectionShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}

void tCubeMapReflectionShader::SetReflectionColor(tVector color)
{
	glUniform3f(reflection_color_uniform, color.x, color.y, color.z);
}

void tCubeMapReflectionShader::SetCubeMapTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + cube_map_tex_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
}

void tCubeMapReflectionShader::SetTransformation(const float m[16])
{
	glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}
