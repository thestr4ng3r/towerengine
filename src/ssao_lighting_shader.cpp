
#include "towerengine.h"
#include "tresources.h"

void tSSAOLightingShader::Init(tGBuffer *gbuffer)
{
	InitLightingShader(ssao_lighting_shader_frag, "SSAO Lighting Shader");

	ssao_tex_uniform = GetUniformLocation("ssao_tex_uni");
	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");

	light_ambient_color_uniform = GetUniformLocation("light_ambient_color_uni");

	Bind();
	glUniform1i(diffuse_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::DIFFUSE_TEX));

	ssao_tex_unit = gbuffer->GetLastTextureUnit();
	glUniform1i(ssao_tex_uniform, ssao_tex_unit);
}

void tSSAOLightingShader::SetSSAOTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + ssao_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void tSSAOLightingShader::SetAmbientLight(tVector color)
{
	glUniform3f(light_ambient_color_uniform, color.x, color.y, color.z);
}
