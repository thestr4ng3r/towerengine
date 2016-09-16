
#include "towerengine.h"
#include "resources.h"

t2DSpriteShader::t2DSpriteShader(void)
{
	InitShader(resources_get("2d_sprite_shader.vert"), resources_get("2d_sprite_shader.frag"), "2D Sprite Shader");
	glBindAttribLocation(program, 0, "vertex_attr");
	LinkProgram();

	screen_size_uniform = GetUniformLocation("screen_size_uni");
	sprite_coords_uniform = GetUniformLocation("sprite_coords_uni");

	tex_uniform = GetUniformLocation("tex_uni");

	Bind();
	glUniform1i(tex_uniform, 0);
}


void t2DSpriteShader::SetScreenSize(int width, int height)
{
	int v[2] = { width, height };
	glUniform1iv(screen_size_uniform, 2, v);
}

void t2DSpriteShader::SetSprite(int x, int y, int width, int height, GLuint tex)
{
	int v[4] = { x, y, width, height };
	glUniform1iv(sprite_coords_uniform, 4, v);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
}
