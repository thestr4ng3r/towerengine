
#include "towerengine.h"
#include "tresources.h"


void tLightingShader::Init(tGBuffer *gbuffer)
{
	InitScreenShader(lighting_shader_frag, "Lighting Shader");

	position_tex_uniform = GetUniformLocation("position_tex_uni");
	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	specular_tex_uniform = GetUniformLocation("specular_tex_uni");
	self_illumination_tex_uniform = GetUniformLocation("self_illumination_tex_uni");

	ssao_enabled_uniform = GetUniformLocation("ssao_enabled_uni");
	ssao_tex_uniform = GetUniformLocation("ssao_tex_uni");

	light_ambient_color_uniform = GetUniformLocation("light_ambient_color_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");

	// point lighting
	/*point_light_count_uniform = GetUniformLocation("point_light_count_uni");
	point_light_pos_uniform = GetUniformLocation("point_light_pos_uni");
	point_light_color_uniform = GetUniformLocation("point_light_color_uni");
	point_light_distance_uniform = GetUniformLocation("point_light_distance_uni");*/
	//point_light_shadow_enabled_uniform = GetUniformLocation("point_light_shadow_enabled_uni");
	point_light_shadow_map_uniform = GetUniformLocation("point_light_shadow_map_uni");


	Bind();
	glUniform1i(position_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::POSITION_TEX));
	glUniform1i(diffuse_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::DIFFUSE_TEX));
	glUniform1i(normal_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::NORMAL_TEX));
	glUniform1i(specular_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::SPECULAR_TEX));
	glUniform1i(self_illumination_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::SELF_ILLUMINATION_TEX));
}

void tLightingShader::SetAmbientLight(tVector color)
{
	glUniform3f(light_ambient_color_uniform, color.x, color.y, color.z);
}


void tLightingShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}


void tLightingShader::SetSSAO(bool enabled, GLuint64 tex_handle)
{
	glUniform1i(ssao_enabled_uniform, enabled ? 1 : 0);
	if(enabled)
		glUniformHandleui64ARB(ssao_tex_uniform, tex_handle);
}


/*void tLightingShader::SetPointLights(int count, float *pos, float *color, float *dist, int *shadow_enabled, GLuint64 *shadow_map_handles)
{
	glUniform1i(point_light_count_uniform, count);
	glUniform3fv(point_light_pos_uniform, count, pos);
	glUniform3fv(point_light_color_uniform, count, color);
	glUniform1fv(point_light_distance_uniform, count, dist);

	glUniform1iv(point_light_shadow_enabled_uniform, count, shadow_enabled);
	glUniformHandleui64vARB(point_light_shadow_map_uniform, count, shadow_map_handles);
}*/
