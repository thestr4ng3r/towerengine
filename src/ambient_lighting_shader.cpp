
#include "towerengine.h"


void tAmbientLightingShader::Init(void)
{
	InitShader(light_pass_shader_vert, ambient_lighting_shader_frag, "Ambient Lighting Shader");
	LinkProgram();

	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");

	light_ambient_color_uniform = GetUniformLocation("light_ambient_color_uni");
}

void tAmbientLightingShader::SetGBuffer(tGBuffer *gbuffer)
{
	glUniform1i(diffuse_tex_uniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbuffer->GetTexture(tGBuffer::DIFFUSE_TEX));
}

void tAmbientLightingShader::SetAmbientLight(tVector color)
{
	glUniform3f(light_ambient_color_uniform, color.x, color.y, color.z);
}
