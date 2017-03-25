
#include "towerengine.h"

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE

#include "resources.h"
#include "shader_source.h"


using namespace std;


tLightingShader::tLightingShader(tGBuffer *gbuffer)
{
	tShaderSource src(resources_get("lighting_shader.frag"));
	src.SetParameter("max_point_lights_count", new tShaderSourceVariable(max_point_lights_count));
	InitScreenShader(src.BuildSource().c_str(), "Lighting Shader");

	depth_tex_uniform = GetUniformLocation("depth_tex_uni");
	base_color_tex_uniform = GetUniformLocation("base_color_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	metallic_roughness_tex_uniform = GetUniformLocation("metallic_roughness_tex_uni");
	emission_tex_uniform = GetUniformLocation("emission_tex_uni");

	ssao_enabled_uniform = GetUniformLocation("ssao_enabled_uni");
	ssao_tex_uniform = GetUniformLocation("ssao_tex_uni");

	light_ambient_color_uniform = GetUniformLocation("light_ambient_color_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	reflection_tex1_uniform = GetUniformLocation("reflection_tex1_uni");
	reflection_tex2_uniform = GetUniformLocation("reflection_tex2_uni");
	reflection_tex_blend_uniform = GetUniformLocation("reflection_tex_blend_uni");


	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "PointLightBlock"), point_light_binding_point);
	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "PositionRestoreDataBlock"), position_restore_data_binding_point);


	Bind();
	glUniform1i(depth_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::DEPTH_TEX));
	glUniform1i(base_color_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::BASE_COLOR_TEX));
	glUniform1i(normal_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::NORMAL_TEX));
	glUniform1i(metallic_roughness_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::METAL_ROUGH_REFLECT_TEX));
	glUniform1i(emission_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::EMISSION_TEX));

	reflection_tex1_unit = gbuffer->GetLastTextureUnit() + 1;
	glUniform1i(reflection_tex1_uniform, reflection_tex1_unit);

	reflection_tex2_unit = gbuffer->GetLastTextureUnit() + 2;
	glUniform1i(reflection_tex2_uniform, reflection_tex2_unit);

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

void tLightingShader::SetReflectionTextures(GLuint tex1, GLuint tex2, float blend)
{
	glActiveTexture(GL_TEXTURE0 + reflection_tex1_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex1);

	glActiveTexture(GL_TEXTURE0 + reflection_tex2_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex2);

	glUniform1f(reflection_tex_blend_uniform, blend);
}



// -------------------------------

tLightingShaderPointLightsBuffer::tLightingShaderPointLightsBuffer(void)
	: tUniformBuffer(16+4*16*tLightingShader::max_point_lights_count)
{
}

tLightingShaderPointLightsBuffer::~tLightingShaderPointLightsBuffer(void)
{
}



void tLightingShaderPointLightsBuffer::UpdateBuffer(std::vector<tPointLight *> lights)
{
	int lights_count = lights.size();
	if(lights_count > tLightingShader::max_point_lights_count)
		lights_count = tLightingShader::max_point_lights_count;

	((int *)(&data[0*16]))[0] = lights_count;

	vector<tPointLight *>::iterator point_light_it;
	int point_light_i = 0;

	for(point_light_it=lights.begin(); point_light_it!=lights.end() && point_light_i<lights_count; point_light_it++, point_light_i++)
	{
		tPointLight *light = *point_light_it;

		unsigned int buffer_pos = 16 + point_light_i*4*16;

		((float *)(&data[buffer_pos]))[0] = light->GetDistance();
		((int *)(&data[buffer_pos]))[1] = light->GetShadowEnabled() ? 1 : 0;
		((float *)(&data[buffer_pos+16]))[0] = light->GetPosition().x;
		((float *)(&data[buffer_pos+16]))[1] = light->GetPosition().y;
		((float *)(&data[buffer_pos+16]))[2] = light->GetPosition().z;
		((float *)(&data[buffer_pos+2*16]))[0] = light->GetColor().x;
		((float *)(&data[buffer_pos+2*16]))[1] = light->GetColor().y;
		((float *)(&data[buffer_pos+2*16]))[2] = light->GetColor().z;

		GLuint64 shadow_handle = 0;
		if(light->GetShadow())
		{
			light->GetShadow()->MakeTextureHandleResident(true);
			shadow_handle = light->GetShadow()->GetTextureHandle();
		}
		((GLuint64 *)(&data[buffer_pos+3*16]))[0] = shadow_handle;
	}

	UploadData();
}

void tLightingShaderPointLightsBuffer::Bind(void)
{
	tUniformBuffer::Bind(tShader::point_light_binding_point);
}


#endif







