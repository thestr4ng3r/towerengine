
#include "towerengine.h"

using namespace std;

tVRForwardRenderer::tVRForwardRenderer(int left_width, int left_height, int right_width, int right_height, unsigned int msaa_samples, tWorld *world)
{
	width[0] = left_width;
	height[0] = left_height;
	width[1] = right_width;
	height[1] = right_height;

	InitForwardRenderer(world);

	for(int i=0; i<2; i++)
		camera[i] = new tCamera();

	camera_render_space = new tRenderSpace();



	fb_width = left_width + right_width;
	fb_height = max(left_height, right_height);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GLenum tex_target = GL_TEXTURE_2D;

	if(msaa_samples == 0)
	{
		msaa_enabled = false;
	}
	else
	{
		msaa_enabled = true;
		tex_target = GL_TEXTURE_2D_MULTISAMPLE;
	}

	glGenTextures(1, &color_tex);
	glBindTexture(tex_target, color_tex);
	if(msaa_enabled)
		glTexImage2DMultisample(tex_target, msaa_samples, GL_RGBA8, fb_width, fb_height, GL_FALSE);
	else
	{
		glTexParameteri(tex_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(tex_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(tex_target, 0, GL_RGBA, fb_width, fb_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_target, color_tex, 0);

	glGenTextures(1, &depth_tex);
	glBindTexture(tex_target, depth_tex);
	if(msaa_enabled)
		glTexImage2DMultisample(tex_target, msaa_samples, GL_DEPTH_COMPONENT24, fb_width, fb_height, GL_FALSE);
	else
	{
		glTexParameteri(tex_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(tex_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(tex_target, 0, GL_DEPTH_COMPONENT24, fb_width, fb_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	}
	//glTexParameteri(tex_target, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex_target, depth_tex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

tVRForwardRenderer::~tVRForwardRenderer(void)
{
	for(int i=0; i<2; i++)
		delete camera[i];
}


void tVRForwardRenderer::PrepareVRRender(void)
{
	for(int eye=0; eye<2; eye++)
	{
		tCamera *cam = camera[eye];

		//cam->SetAspect((float)width[eye] / (float)height[eye]);
		cam->CalculateModelViewProjectionMatrix();
	}

	world->FillRenderSpace(camera_render_space, (tCulling **)camera, 2);

	tForwardRenderer::PrepareRender(camera[0], camera_render_space); // TODO: use something instead of left camera
}

void tVRForwardRenderer::Render(GLuint dst_fbo)
{
	PrepareVRRender();

	// render...

	current_rendering_render_space = camera_render_space;

	matrix_buffer->Bind();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if(msaa_enabled)
		glEnable(GL_MULTISAMPLE);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	for(int eye = 0; eye < 2; eye++)
	{
		current_rendering_camera = camera[eye];
		matrix_buffer->UpdateBuffer(current_rendering_camera->GetModelViewProjectionMatrix());

		glViewport(eye == 1 ? width[0] : 0, 0, width[eye], height[eye]);

		RenderForward();
	}

	if(msaa_enabled)
		glDisable(GL_MULTISAMPLE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst_fbo);
	glBlitFramebuffer(0, 0, fb_width, fb_height, 0, 0, fb_width, fb_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
