
#include "towerengine.h"


t2DRenderer::t2DRenderer(void)
{
	sprite_shader = new t2DSpriteShader();
	sprite_shader->Init();


	vao = new tVAO();
	vao->Bind();

	vertex_vbo = new tVBO<float>(2, 4);
	float vertex_data[8] = {
			0.0, 0.0,
			0.0, 1.0,
			1.0, 1.0,
			1.0, 0.0
		};
	memcpy(vertex_vbo->GetData(), vertex_data, sizeof(float) * 8);
	vertex_vbo->AssignData();
	vertex_vbo->SetAttribute(tShader::vertex_attribute, GL_FLOAT);

	ibo = new tIBO(6);
	int index_data[6] = { 0, 1, 2, 0, 2, 3 };
	memcpy(ibo->GetData(), index_data, sizeof(int) * 6);
	ibo->AssignData();
}

void t2DRenderer::InitRender(int screen_width, int screen_height)
{
	sprite_shader->Bind();
	sprite_shader->SetScreenSize(screen_width, screen_height);

	vao->Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
}

void t2DRenderer::RenderSprite(int x, int y, int width, int height, GLuint tex)
{
	sprite_shader->SetSprite(x, y, width, height, tex);

	vao->Bind();
	ibo->Draw(GL_TRIANGLES);
}

