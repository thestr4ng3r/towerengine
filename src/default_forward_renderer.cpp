
#include "towerengine.h"

tDefaultForwardRenderer::tDefaultForwardRenderer(tWorld *world)
{
	InitForwardRenderer(world);

	camera = new tCamera();
	camera_render_space = new tRenderSpace();
}


tDefaultForwardRenderer::~tDefaultForwardRenderer(void)
{
	delete camera;
	delete camera_render_space;
}

void tDefaultForwardRenderer::Render(GLuint dst_fbo, int viewport_x, int viewport_y, int viewport_width, int viewport_height)
{
	camera->CalculateModelViewProjectionMatrix();
	world->FillRenderSpace(camera_render_space, (tCulling **)&camera, 1);

	PrepareRender(camera, camera_render_space);

	// render...

	current_rendering_camera = camera;
	current_rendering_render_space = camera_render_space;

	matrix_buffer->Bind();
	matrix_buffer->UpdateBuffer(current_rendering_camera->GetModelViewProjectionMatrix());


	glBindFramebuffer(GL_FRAMEBUFFER, dst_fbo);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(viewport_x, viewport_y, viewport_width, viewport_height);

	StandardForwardPass();

	tShader::Unbind();

	if(dst_fbo != 0)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

}