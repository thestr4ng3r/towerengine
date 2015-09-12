
#include "towerengine.h"
#include "tresources.h"


void tDirectionalLightingShader::Init(tGBuffer *gbuffer)
{
	InitScreenShader(directional_lighting_shader_frag, "Directional Lighting Shader");

	position_tex_uniform = GetUniformLocation("position_tex_uni");
	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	specular_tex_uniform = GetUniformLocation("specular_tex_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	directional_light_dir_uniform = GetUniformLocation("directional_light_dir_uni");
	directional_light_color_uniform = GetUniformLocation("directional_light_color_uni");
	directional_light_shadow_enabled_uniform = GetUniformLocation("directional_light_shadow_enabled_uni");
	directional_light_shadow_clip_uniform = GetUniformLocation("directional_light_shadow_clip_uni");
	directional_light_shadow_tex_matrix_uniform = GetUniformLocation("directional_light_shadow_tex_matrix_uni");
	directional_light_shadow_splits_count_uniform = GetUniformLocation("directional_light_shadow_splits_count_uni");
	directional_light_shadow_splits_z_uniform = GetUniformLocation("directional_light_shadow_splits_z_uni");
	directional_light_shadow_map_uniform = GetUniformLocation("directional_light_shadow_map_uni");

	Bind();
	glUniform1i(position_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::POSITION_TEX));
	glUniform1i(diffuse_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::DIFFUSE_TEX));
	glUniform1i(normal_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::NORMAL_TEX));
	glUniform1i(specular_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::SPECULAR_TEX));

	directional_light_shadow_tex_unit = gbuffer->GetLastTextureUnit() + 1;
	glUniform1i(directional_light_shadow_map_uniform, directional_light_shadow_tex_unit);
}

void tDirectionalLightingShader::SetDirectionalLight(tVector dir, tVector color, int shadow_enabled, GLuint shadow_map, tVector2 shadow_clip, float *shadow_tex_matrix, int shadow_splits_count, float *shadow_splits_z)
{
	glUniform3f(directional_light_dir_uniform, dir.x, dir.y, dir.z);
	glUniform3f(directional_light_color_uniform, color.x, color.y, color.z);
	glUniform1i(directional_light_shadow_enabled_uniform, shadow_enabled);
	glUniform2f(directional_light_shadow_clip_uniform, shadow_clip.x, shadow_clip.y);
	glUniformMatrix4fv(directional_light_shadow_tex_matrix_uniform, shadow_splits_count, GL_FALSE, shadow_tex_matrix);
	glUniform1i(directional_light_shadow_splits_count_uniform, shadow_splits_count);
	glUniform1fv(directional_light_shadow_splits_z_uniform, shadow_splits_count+1, shadow_splits_z);

	glActiveTexture(GL_TEXTURE0 + directional_light_shadow_tex_unit);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_map);
}

void tDirectionalLightingShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}

