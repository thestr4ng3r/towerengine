
#include "towerengine.h"
#include "tresources.h"

void tPointLightingShader::Init(tGBuffer *gbuffer)
{
	InitLightingShader(point_lighting_shader_frag, "Point Lighting Shader");

	position_tex_uniform = GetUniformLocation("position_tex_uni");
	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	specular_tex_uniform = GetUniformLocation("specular_tex_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	point_light_pos_uniform = GetUniformLocation("point_light_pos_uni");
	point_light_color_uniform = GetUniformLocation("point_light_color_uni");
	point_light_distance_uniform = GetUniformLocation("point_light_distance_uni");
	point_light_shadow_map_uniform = GetUniformLocation("point_light_shadow_map_uni");
	point_light_shadow_enabled_uniform = GetUniformLocation("point_light_shadow_enabled_uni");

	Bind();
	glUniform1i(position_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::POSITION_TEX));
	glUniform1i(diffuse_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::DIFFUSE_TEX));
	glUniform1i(normal_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::NORMAL_TEX));
	glUniform1i(specular_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::SPECULAR_TEX));

	point_light_shadow_tex_unit = gbuffer->GetLastTextureUnit() + 1;
	glUniform1i(point_light_shadow_map_uniform, point_light_shadow_tex_unit);
}

void tPointLightingShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}

void tPointLightingShader::SetPointLight(tVector pos, tVector color, float dist, int shadow_enabled, GLuint shadow_map)
{
	glUniform3f(point_light_pos_uniform, pos.x, pos.y, pos.z);
	glUniform3f(point_light_color_uniform, color.x, color.y, color.z);
	glUniform1f(point_light_distance_uniform, dist);

	glUniform1i(point_light_shadow_enabled_uniform, shadow_enabled);

	glActiveTexture(GL_TEXTURE0 + point_light_shadow_tex_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_map);
}
