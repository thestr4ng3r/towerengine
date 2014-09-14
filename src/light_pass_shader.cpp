
#include "towerengine.h"

void CLightPassShader::Init(void)
{
	InitShader(light_pass_shader_vert, light_pass_shader_frag, "Light Pass Shader");
	LinkProgram();

	position_tex_uniform = GetUniformLocation("position_tex_uni");
	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	specular_tex_uniform = GetUniformLocation("specular_tex_uni");
	specular_exponent_tex_uniform = GetUniformLocation("specular_exponent_tex_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	point_light_count_uniform = GetUniformLocation("point_light_count_uni");
	point_light_pos_uniform = GetUniformLocation("point_light_pos_uni");
	point_light_color_uniform = GetUniformLocation("point_light_color_uni");
	point_light_distance_uniform = GetUniformLocation("point_light_distance_uni");
	point_light_shadow_map_uniform = GetUniformLocation("point_light_shadow_map_uni");
	point_light_shadow_enabled_uniform = GetUniformLocation("point_light_shadow_enabled_uni");

	directional_light_count_uniform = GetUniformLocation("directional_light_count_uni");
	directional_light_dir_uniform = GetUniformLocation("directional_light_dir_uni");
	directional_light_color_uniform = GetUniformLocation("directional_light_color_uni");
	directional_light_shadow_enabled_uniform = GetUniformLocation("directional_light_shadow_enabled_uni");
	directional_light_shadow_clip_uniform = GetUniformLocation("directional_light_shadow_clip_uni");
	directional_light_shadow_tex_matrix_uniform = GetUniformLocation("directional_light_shadow_tex_matrix_uni");
	directional_light_shadow_splits_count_uniform = GetUniformLocation("directional_light_shadow_splits_count_uni");
	directional_light_shadow_splits_z_uniform = GetUniformLocation("directional_light_shadow_splits_z_uni");
	directional_light_shadow_map_uniform = GetUniformLocation("directional_light_shadow_map_uni");

	light_ambient_color_uniform = GetUniformLocation("light_ambient_color_uni");
}

void CLightPassShader::SetGBuffer(CGBuffer *gbuffer)
{
	glUniform1iARB(position_tex_uniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbuffer->GetTexture(CGBuffer::POSITION_TEX));

	glUniform1iARB(diffuse_tex_uniform, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gbuffer->GetTexture(CGBuffer::DIFFUSE_TEX));

	glUniform1iARB(normal_tex_uniform, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gbuffer->GetTexture(CGBuffer::NORMAL_TEX));

	glUniform1iARB(specular_tex_uniform, 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gbuffer->GetTexture(CGBuffer::SPECULAR_TEX));

	glUniform1iARB(specular_exponent_tex_uniform, 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gbuffer->GetTexture(CGBuffer::SPECULAR_EXPONENT_TEX));
}

void CLightPassShader::SetPointLights(int count, float *pos, float *color, float *dist, int *shadow_enabled, GLuint *shadow_maps)
{
	int i;
	count = min(count, max_point_lights);
	glUniform1iARB(point_light_count_uniform, count);
	glUniform3fvARB(point_light_pos_uniform, count, pos);
	glUniform3fvARB(point_light_color_uniform, count, color);
	glUniform1fvARB(point_light_distance_uniform, count, dist);

	glUniform1ivARB(point_light_shadow_enabled_uniform, count, shadow_enabled);
	int *tex_units = new int[max_point_lights];
	for(i=0; i<count; i++)
	{
		tex_units[i] = point_light_shadow_tex_first_unit + i;
		glActiveTextureARB(GL_TEXTURE0 + tex_units[i]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_maps[i]);
	}
	for(i=count; i<max_point_lights; i++)
		tex_units[i] = point_light_shadow_tex_first_unit + i;

	glUniform1ivARB(point_light_shadow_map_uniform, max_point_lights, tex_units);
}

void CLightPassShader::SetDirectionalLights(int count, float *dir, float *color, int *shadow_enabled, GLuint *shadow_maps, float *shadow_clip, float *shadow_tex_matrix, float *shadow_splits_count, float *shadow_splits_z)
{
	int i;

	count = min(count, max_directional_lights);
	glUniform1iARB(directional_light_count_uniform, count);
	glUniform3fvARB(directional_light_dir_uniform, count, dir);
	glUniform3fvARB(directional_light_color_uniform, count, color);
	glUniform1ivARB(directional_light_shadow_enabled_uniform, count, shadow_enabled);
	glUniform2fvARB(directional_light_shadow_clip_uniform, count, shadow_clip);
	glUniformMatrix4fvARB(directional_light_shadow_tex_matrix_uniform, count * max_directional_light_splits, GL_FALSE, shadow_tex_matrix);
	glUniform1fvARB(directional_light_shadow_splits_count_uniform, count, shadow_splits_count);
	glUniform1fvARB(directional_light_shadow_splits_z_uniform, count * (max_directional_light_splits+1), shadow_splits_z);

	int *tex_units = new int[max_directional_lights];
	for(i=0; i<count; i++)
	{
		tex_units[i] = directional_light_shadow_tex_first_unit + i;
		glActiveTextureARB(GL_TEXTURE0 + tex_units[i]);
		glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_maps[i]);
	}
	for(i=count; i<max_directional_lights; i++)
		tex_units[i] = directional_light_shadow_tex_first_unit + i;

	glUniform1ivARB(directional_light_shadow_map_uniform, max_directional_lights, tex_units);
}

void CLightPassShader::SetLightAmbientColor(CVector color)
{
	glUniform3fARB(light_ambient_color_uniform, color.x, color.y, color.z);
}

void CLightPassShader::SetCameraPosition(CVector pos)
{
	glUniform3fARB(cam_pos_uniform, pos.x, pos.y, pos.z);
}

