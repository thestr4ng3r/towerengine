
#include "towerengine.h"
#include "resources.h"

void tPostProcessShader::Init(void)
{
	InitScreenShader(get_resource("post_process_shader.frag"), "Post Process Shader");

	color_tex_uniform = GetUniformLocation("color_tex_uni");
	tex_pixel_uniform = GetUniformLocation("tex_pixel_uni");

	fxaa_enabled_uniform = GetUniformLocation("fxaa_enabled_uni");

	Bind();
	glUniform1i(color_tex_uniform, 0);
}

void tPostProcessShader::SetTextures(GLuint color, int width, int height)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color);

	glUniform2f(tex_pixel_uniform, 1.0f / (float)width, 1.0f / (float)height);
}

void tPostProcessShader::SetFXAA(bool enabled)
{
	glUniform1i(fxaa_enabled_uniform, enabled ? 1 : 0);
}
