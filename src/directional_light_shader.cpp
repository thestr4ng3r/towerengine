
#include "towerengine.h"


void tDirectionalLightShader::Init(void)
{
	InitShader(light_pass_shader_vert, directional_light_shader_frag, "Directional Light Shader");
	LinkProgram();

	position_tex_uniform = GetUniformLocation("position_tex_uni");
	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	specular_tex_uniform = GetUniformLocation("specular_tex_uni");
	specular_exponent_tex_uniform = GetUniformLocation("specular_exponent_tex_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	directional_light_dir_uniform = GetUniformLocation("directional_light_dir_uni");
	directional_light_color_uniform = GetUniformLocation("directional_light_color_uni");
	directional_light_shadow_enabled_uniform = GetUniformLocation("directional_light_shadow_enabled_uni");
	directional_light_shadow_clip_uniform = GetUniformLocation("directional_light_shadow_clip_uni");
	directional_light_shadow_tex_matrix_uniform = GetUniformLocation("directional_light_shadow_tex_matrix_uni");
	directional_light_shadow_splits_count_uniform = GetUniformLocation("directional_light_shadow_splits_count_uni");
	directional_light_shadow_splits_z_uniform = GetUniformLocation("directional_light_shadow_splits_z_uni");
	directional_light_shadow_map_uniform = GetUniformLocation("directional_light_shadow_map_uni");
}

void tDirectionalLightShader::SetGBuffer(tGBuffer *gbuffer)
{
	glUniform1i(position_tex_uniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbuffer->GetTexture(tGBuffer::POSITION_TEX));

	glUniform1i(diffuse_tex_uniform, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gbuffer->GetTexture(tGBuffer::DIFFUSE_TEX));

	glUniform1i(normal_tex_uniform, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gbuffer->GetTexture(tGBuffer::NORMAL_TEX));

	glUniform1i(specular_tex_uniform, 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gbuffer->GetTexture(tGBuffer::SPECULAR_TEX));

	glUniform1i(specular_exponent_tex_uniform, 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gbuffer->GetTexture(tGBuffer::SPECULAR_EXPONENT_TEX));
}

void tDirectionalLightShader::SetDirectionalLight(tVector dir, tVector color, int shadow_enabled, GLuint shadow_map, tVector2 shadow_clip, float *shadow_tex_matrix, int shadow_splits_count, float *shadow_splits_z)
{
	int i;

	glUniform3f(directional_light_dir_uniform, dir.x, dir.y, dir.z);
	glUniform3f(directional_light_color_uniform, color.x, color.y, color.z);
	glUniform1i(directional_light_shadow_enabled_uniform, shadow_enabled);
	glUniform2f(directional_light_shadow_clip_uniform, shadow_clip.x, shadow_clip.y);
	glUniformMatrix4fv(directional_light_shadow_tex_matrix_uniform, shadow_splits_count, GL_FALSE, shadow_tex_matrix);
	glUniform1i(directional_light_shadow_splits_count_uniform, shadow_splits_count);
	glUniform1fv(directional_light_shadow_splits_z_uniform, shadow_splits_count+1, shadow_splits_z);

	glUniform1i(directional_light_shadow_map_uniform, directional_light_shadow_tex_unit);
	glActiveTexture(GL_TEXTURE0 + directional_light_shadow_tex_unit);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_map);
}

void tDirectionalLightShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}

