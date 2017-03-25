
#include "towerengine.h"

void tForwardRenderer::InitForwardRenderer(tWorld *world)
{
	InitRenderer(world);

	standard_shader = new tStandardForwardShader();
}

tForwardRenderer::~tForwardRenderer()
{
	delete standard_shader;
}

void tForwardRenderer::PrepareRender(tCamera *camera, tRenderSpace *render_space)
{
	tRenderer::PrepareRender(camera, render_space);
}

void tForwardRenderer::StandardForwardPass()
{
	SetCurrentFaceShader(standard_shader);
	BindCurrentFaceShader();

	glEnable(GL_DEPTH_TEST);

	current_rendering_render_space->StandardForwardPass(this);
}