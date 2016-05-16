
#include "towerengine.h"

tDefaultRenderer::tDefaultRenderer(int width, int height, tWorld *world)
{
	InitRenderer(width, height, world, true);

	camera = new tCamera();
	camera_render_space = new tRenderSpace();
}


tDefaultRenderer::~tDefaultRenderer(void)
{
	delete camera;
	delete camera_render_space;
}

void tDefaultRenderer::Render(GLuint dst_fbo, int viewport_x, int viewport_y, int viewport_width, int viewport_height)
{
	//camera->SetAspect((float)screen_width / (float)screen_height);
	camera->CalculateModelViewProjectionMatrix();

	world->FillRenderSpace(camera_render_space, (tCulling **)&camera, 1);

	tRenderer::PrepareRender(camera, camera_render_space);
	tRenderer::Render(camera, camera_render_space, dst_fbo, viewport_x, viewport_y, viewport_width, viewport_height);
}
