
#include "towerengine.h"

tDefaultDeferredRenderer::tDefaultDeferredRenderer(int width, int height, tWorld *world)
{
	InitDeferredRenderer(width, height, world, true);

	camera = new tCamera();
	camera_render_space = new tRenderObjectSpace();
}


tDefaultDeferredRenderer::~tDefaultDeferredRenderer(void)
{
	delete camera;
	delete camera_render_space;
}

void tDefaultDeferredRenderer::Render(GLuint dst_fbo, int viewport_x, int viewport_y, int viewport_width, int viewport_height)
{
	camera->CalculateModelViewProjectionMatrix();
	tCulling *cullings[1] = { camera };
	world->FillObjectSpace(camera_render_space, cullings, 1);

	PrepareRender(camera, camera_render_space);


	// render...

	current_rendering_camera = camera;
	current_rendering_render_space = camera_render_space;

	matrix_buffer->Bind();
	matrix_buffer->UpdateBuffer(camera->GetModelViewProjectionMatrix());

	position_restore_data_buffer->Bind();


	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	gbuffer->BindDrawBuffers();
	glViewport(0, 0, screen_width, screen_height);

	glClear(GL_DEPTH_BUFFER_BIT);
	float clear_color[] = { 0.0, 0.0, 0.0, 0.0 };
	glClearBufferfv(GL_COLOR, gbuffer->GetDrawBufferIndex(tGBuffer::BASE_COLOR_TEX), clear_color);

	GeometryPass();

	position_restore_data_buffer->UpdateBuffer(current_rendering_camera);

	if(ssao)
	{
		ssao->Render(); // binds its own fbo
		glBindFramebuffer(GL_FRAMEBUFFER, fbo); // rebind fbo
	}

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	LightPass();

	ForwardPass();

	current_read_color_tex = 0;

	glReadBuffer(GL_COLOR_ATTACHMENT0 + current_read_color_tex);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + (1 - current_read_color_tex));
	glBlitFramebuffer(0, 0, screen_width, screen_height, 0, 0, screen_width, screen_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	RefractionPass();
	current_read_color_tex = 1 - current_read_color_tex;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	if(fog_enabled)
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + (1 - current_read_color_tex));

		fog_shader->Bind();
		fog_shader->SetTextures(gbuffer->GetTexture(tGBuffer::DEPTH_TEX), color_tex[current_read_color_tex]);
		fog_shader->SetCameraPosition(camera->GetPosition());

		RenderScreenQuad();

		current_read_color_tex = 1 - current_read_color_tex;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, dst_fbo);


	if(viewport_width > 0 && viewport_height > 0)
		glViewport(viewport_x, viewport_y, viewport_width, viewport_height);

	post_process_shader->Bind();
	post_process_shader->SetFXAA(fxaa_enabled);
	post_process_shader->SetTextures(color_tex[current_read_color_tex], screen_width, screen_height);

	RenderScreenQuad();

	tShader::Unbind();

	if(dst_fbo != 0)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
