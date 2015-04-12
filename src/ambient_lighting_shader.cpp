
#include "towerengine.h"
#include "tresources.h"


void tAmbientLightingShader::Init(void)
{
	InitLightingShader(ambient_lighting_shader_frag, "Ambient Lighting Shader");

	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");

	light_ambient_color_uniform = GetUniformLocation("light_ambient_color_uni");

	Bind();
	glUniform1i(diffuse_tex_uniform, 0);
}

void tAmbientLightingShader::SetGBuffer(tGBuffer *gbuffer)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbuffer->GetTexture(tGBuffer::DIFFUSE_TEX));
}

void tAmbientLightingShader::SetAmbientLight(tVector color)
{
	glUniform3f(light_ambient_color_uniform, color.x, color.y, color.z);
}
