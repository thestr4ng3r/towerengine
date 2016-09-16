
#include "towerengine.h"
#include "resources.h"


tAmbientLightingShader::tAmbientLightingShader(tGBuffer *gbuffer, bool ambient_ssao)
{
	const char *src;
	if(ambient_ssao)
		src = resources_get("ssao_ambient_lighting_shader.frag");
	else
		src = resources_get("ambient_lighting_shader.frag");

	InitScreenShader(src, "Ambient Lighting Shader");

	depth_tex_uniform = GetUniformLocation("depth_tex_uni");
	base_color_tex_uniform = GetUniformLocation("base_color_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	metal_rough_reflect_tex_uniform = GetUniformLocation("metal_rough_reflect_tex_uni");
	emission_tex_uniform = GetUniformLocation("emission_tex_uni");

	if(ambient_ssao)
		ssao_tex_uniform = GetUniformLocation("ssao_tex_uni");
	else
		ssao_tex_uniform = -1;

	light_ambient_color_uniform = GetUniformLocation("light_ambient_color_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	reflection_tex1_uniform = GetUniformLocation("reflection_tex1_uni");
	reflection_tex2_uniform = GetUniformLocation("reflection_tex2_uni");
	reflection_tex_blend_uniform = GetUniformLocation("reflection_tex_blend_uni");


	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "PositionRestoreDataBlock"), position_restore_data_binding_point);


	Bind();
	glUniform1i(depth_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::DEPTH_TEX));
	glUniform1i(base_color_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::BASE_COLOR_TEX));
	glUniform1i(normal_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::NORMAL_TEX));
	glUniform1i(metal_rough_reflect_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::METAL_ROUGH_REFLECT_TEX));
	glUniform1i(emission_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::EMISSION_TEX));


	reflection_tex1_unit = gbuffer->GetLastTextureUnit() + 1;
	glUniform1i(reflection_tex1_uniform, reflection_tex1_unit);

	reflection_tex2_unit = gbuffer->GetLastTextureUnit() + 2;
	glUniform1i(reflection_tex2_uniform, reflection_tex2_unit);

	if(ambient_ssao)
	{
		ssao_tex_unit = gbuffer->GetLastTextureUnit() + 3;
		glUniform1i(ssao_tex_uniform, ssao_tex_unit);
	}
	else
		ssao_tex_unit = 0;
}

void tAmbientLightingShader::SetAmbientLight(tVector color)
{
	glUniform3f(light_ambient_color_uniform, color.x, color.y, color.z);
}


void tAmbientLightingShader::SetSSAOTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + ssao_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void tAmbientLightingShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}

void tAmbientLightingShader::SetReflectionTextures(GLuint tex1, GLuint tex2, float blend)
{
	glActiveTexture(GL_TEXTURE0 + reflection_tex1_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex1);

	glActiveTexture(GL_TEXTURE0 + reflection_tex2_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex2);

	glUniform1f(reflection_tex_blend_uniform, blend);
}