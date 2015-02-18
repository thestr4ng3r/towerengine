
#include "towerengine.h"

void tSSAOLightingShader::Init(void)
{
	InitLightingShader(ssao_lighting_shader_frag, "SSAO Lighting Shader");

	ssao_tex_uniform = GetUniformLocation("ssao_tex_uni");
	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");

	light_ambient_color_uniform = GetUniformLocation("light_ambient_color_uni");
}

void tSSAOLightingShader::SetSSAOTexture(GLuint tex)
{
	glUniform1i(ssao_tex_uniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void tSSAOLightingShader::SetGBuffer(tGBuffer *gbuffer)
{
	glUniform1i(diffuse_tex_uniform, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gbuffer->GetTexture(tGBuffer::DIFFUSE_TEX));
}

void tSSAOLightingShader::SetAmbientLight(tVector color)
{
	glUniform3f(light_ambient_color_uniform, color.x, color.y, color.z);
}
