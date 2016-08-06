
#include "towerengine.h"
#include "resources.h"


void tAmbientLightingShader::Init(tGBuffer *gbuffer)
{
	InitScreenShader(resources_get("ambient_lighting_shader.frag"), "Ambient Lighting Shader");

	depth_tex_uniform = GetUniformLocation("depth_tex_uni");
	base_color_tex_uniform = GetUniformLocation("base_color_tex_uni");
	emission_tex_uniform = GetUniformLocation("emission_tex_uni");

	light_ambient_color_uniform = GetUniformLocation("light_ambient_color_uni");

	Bind();
	glUniform1i(depth_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::DEPTH_TEX));
	glUniform1i(base_color_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::BASE_COLOR_TEX));
	glUniform1i(emission_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::EMISSION_TEX));
}

void tAmbientLightingShader::SetAmbientLight(tVector color)
{
	glUniform3f(light_ambient_color_uniform, color.x, color.y, color.z);
}
