
#include "towerengine.h"
#include "resources.h"

void tSSAOLightingShader::Init(tGBuffer *gbuffer)
{
	InitScreenShader(get_resource("ssao_lighting_shader.frag"), "SSAO Lighting Shader");

	depth_tex_uniform = GetUniformLocation("depth_tex_uni");
	ssao_tex_uniform = GetUniformLocation("ssao_tex_uni");

	Bind();

	ssao_tex_unit = gbuffer->GetLastTextureUnit() + 1;
	glUniform1i(depth_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::DEPTH_TEX));
	glUniform1i(ssao_tex_uniform, ssao_tex_unit);
}

void tSSAOLightingShader::SetSSAOTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0 + ssao_tex_unit);
	glBindTexture(GL_TEXTURE_2D, tex);
}
