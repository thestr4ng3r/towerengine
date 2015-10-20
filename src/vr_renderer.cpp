
#include "towerengine.h"

using namespace std;

tVRRenderer::tVRRenderer(int left_width, int left_height, int right_width, int right_height, tWorld *world)
{
	width[0] = left_width;
	height[0] = left_height;
	width[1] = right_width;
	height[1] = right_height;

	InitRenderer(left_width, max(left_height, right_height), world, true);

	for(int i=0; i<2; i++)
	{
		camera[i] = new tCamera();
		camera_render_space[i] = new tRenderSpace();
	}
}

tVRRenderer::~tVRRenderer(void)
{
	for(int i=0; i<2; i++)
	{
		delete camera[i];
		delete camera_render_space[i];
	}
}

void tVRRenderer::Render(int eye, GLuint dst_fbo)
{
	//printf("%d, %d, %d, %d\n", width[0], height[0], width[1], height[1]);

	tCamera *cam = camera[eye];
	tRenderSpace *render_space = camera_render_space[eye];

	cam->SetAspect((float)width[eye] / (float)height[eye]);
	cam->CalculateModelViewProjectionMatrix();

	world->FillRenderSpace(render_space, cam);

	int render_width = screen_width / 2;

	//tRenderer::Render(cam, render_space, dst_fbo, eye*render_width, 0, render_width, screen_height);
	tRenderer::Render(cam, render_space, dst_fbo, 0, 0, width[eye], height[eye]);
}
