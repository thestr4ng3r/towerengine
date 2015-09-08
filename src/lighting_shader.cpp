
#include "towerengine.h"
#include "tresources.h"


void tLightingShader::Init(tGBuffer *gbuffer)
{
	InitScreenShader(lighting_shader_frag, "Lighting Shader");

	position_tex_uniform = GetUniformLocation("position_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	tang_tex_uniform = GetUniformLocation("tang_tex_uni");
	uv_tex_uniform = GetUniformLocation("uv_tex_uni");
	material_tex_uniform = GetUniformLocation("material_tex_uni");

	light_ambient_color_uniform = GetUniformLocation("light_ambient_color_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");

	// point lighting
	point_light_count_uniform = GetUniformLocation("point_light_count_uni");
	point_light_pos_uniform = GetUniformLocation("point_light_pos_uni");
	point_light_color_uniform = GetUniformLocation("point_light_color_uni");
	point_light_distance_uniform = GetUniformLocation("point_light_distance_uni");
	point_light_shadow_map_uniform = GetUniformLocation("point_light_shadow_map_uni");
	point_light_shadow_enabled_uniform = GetUniformLocation("point_light_shadow_enabled_uni");

	Bind();
	glUniform1i(position_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::POSITION_TEX));
	glUniform1i(normal_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::NORMAL_TEX));
	glUniform1i(tang_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::TANG_TEX));
	glUniform1i(uv_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::UV_TEX));
	glUniform1i(material_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::MATERIAL_TEX));
}

void tLightingShader::SetAmbientLight(tVector color)
{
	glUniform3f(light_ambient_color_uniform, color.x, color.y, color.z);
}


void tLightingShader::SetMaterials(std::vector<tDefaultMaterial *> &materials)
{
	int count = materials.size();

	GLuint64 diffuse_textures[count];
	int i=0;
	for(std::vector<tDefaultMaterial *>::iterator it=materials.begin(); it!=materials.end(); it++)
	{
		tDefaultMaterial *mat = *it;
		diffuse_textures[i] = mat->GetTextureHandle(tDefaultMaterial::DIFFUSE);
		i++;
	}

	glUniformHandleui64vARB(diffuse_tex_uniform, count, diffuse_textures);
}

void tLightingShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}


void tLightingShader::SetPointLights(int count, float *pos, float *color, float *dist, int *shadow_enabled, GLuint64 *shadow_map_handles)
{
	glUniform1i(point_light_count_uniform, count);
	glUniform3fv(point_light_pos_uniform, count, pos);
	glUniform3fv(point_light_color_uniform, count, color);
	glUniform1fv(point_light_distance_uniform, count, dist);

	glUniform1iv(point_light_shadow_enabled_uniform, count, shadow_enabled);
	glUniformHandleui64vARB(point_light_shadow_map_uniform, count, shadow_map_handles);
}
