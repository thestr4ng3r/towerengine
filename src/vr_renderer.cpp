
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
	}

	camera_render_space = new tRenderSpace();
}

tVRRenderer::~tVRRenderer(void)
{
	for(int i=0; i<2; i++)
	{
		delete camera[i];
	}
}


void tVRRenderer::PrepareRender(void)
{
	for(int eye=0; eye<2; eye++)
	{
		tCamera *cam = camera[eye];

		cam->SetAspect((float)width[eye] / (float)height[eye]);
		cam->CalculateModelViewProjectionMatrix();
	}

	world->FillRenderSpace(camera_render_space, (tCulling **)camera, 2);

	tRenderer::PrepareRender(camera[0], camera_render_space); // TODO: use something instead of left camera
}

void tVRRenderer::Render(int eye, GLuint dst_fbo)
{
	tRenderer::Render(camera[eye], camera_render_space, dst_fbo, 0, 0, width[eye], height[eye]);
}
