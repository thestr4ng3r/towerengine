
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

void tVRRenderer::Render(GLuint dst_fbo)
{
	printf("%d, %d, %d, %d\n", width[0], height[0], width[1], height[1]);
	for(int i=0; i<2; i++)
	{
		tCamera *cam = camera[i];
		tRenderSpace *render_space = camera_render_space[i];

		cam->SetAspect((float)width[i] / (float)height[i]);
		cam->CalculateModelViewProjectionMatrix();

		world->FillRenderSpace(render_space, cam);

		tRenderer::Render(cam, render_space, dst_fbo, i*width[0], 0, width[i], height[i]);
	}
}
