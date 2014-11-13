
#include "towerengine.h"

void tSSAOLightingShader::Init(void)
{
	InitShader(light_pass_shader_vert, ssao_lighting_shader_frag, "SSAO Light Shader");
	LinkProgram();

	ssao_tex_uniform = GetUniformLocation("ssao_tex_uni");
	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
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
