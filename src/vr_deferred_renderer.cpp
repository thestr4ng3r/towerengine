
#include "towerengine.h"

using namespace std;

tVRDeferredRenderer::tVRDeferredRenderer(int left_width, int left_height, int right_width, int right_height, tWorld *world)
{
	width[0] = left_width;
	height[0] = left_height;
	width[1] = right_width;
	height[1] = right_height;

	InitDeferredRenderer(left_width + right_width, max(left_height, right_height), world, true);

	for(int i=0; i<2; i++)
	{
		camera[i] = new tCamera();
	}

	camera_render_space = new tRenderSpace();
}

tVRDeferredRenderer::~tVRDeferredRenderer(void)
{
	for(int i=0; i<2; i++)
	{
		delete camera[i];
	}
}


void tVRDeferredRenderer::PrepareVRRender(void)
{
	for(int eye=0; eye<2; eye++)
	{
		tCamera *cam = camera[eye];

		//cam->SetAspect((float)width[eye] / (float)height[eye]);
		cam->CalculateModelViewProjectionMatrix();
	}

	world->FillRenderSpace(camera_render_space, (tCulling **)camera, 2);

	PrepareRender(camera[0], camera_render_space); // TODO: use something instead of left camera
}

void tVRDeferredRenderer::Render(GLuint dst_fbo)
{
	PrepareVRRender();

	// render...

	current_rendering_render_space = camera_render_space;

	matrix_buffer->Bind();
	position_restore_data_buffer->Bind();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	gbuffer->BindDrawBuffers();

	glClear(GL_DEPTH_BUFFER_BIT);
	float clear_color[] = { 0.0, 0.0, 0.0, 0.0 };
	glClearBufferfv(GL_COLOR, gbuffer->GetDrawBufferIndex(tGBuffer::BASE_COLOR_TEX), clear_color);

	for(int eye = 0; eye < 2; eye++)
	{
		current_rendering_camera = camera[eye];
		matrix_buffer->UpdateBuffer(current_rendering_camera->GetModelViewProjectionMatrix());

		glViewport(eye == 1 ? width[0] : 0, 0, width[eye], height[eye]);

		GeometryPass();
	}
	






	// screen space

	for(int eye = 0; eye < 2; eye++)
	{
		current_rendering_camera = camera[eye];
		position_restore_data_buffer->UpdateBuffer(current_rendering_camera, 4.0f, eye == 0 ? -0.25f : -0.75f);
		matrix_buffer->UpdateBuffer(current_rendering_camera->GetModelViewProjectionMatrix()); // TODO: necessary? Is this buffer used here?

		/*float *screen_quad_data = screen_quad_vbo->GetData();
		screen_quad_data[0] = (eye == 0 ? -1.0f : 0.0f); screen_quad_data[1] = 1.0f;
		screen_quad_data[2] = (eye == 0 ? -1.0f : 0.0f); screen_quad_data[3] = -1.0f;
		screen_quad_data[4] = (eye == 0 ? 0.0f : 1.0f); screen_quad_data[5] = 1.0f;
		screen_quad_data[6] = (eye == 0 ? 0.0f : 1.0f); screen_quad_data[7] = -1.0f;
		screen_quad_vbo->AssignData();*/

		float *screen_quad_data = screen_quad_uv_vbo->GetData();
		screen_quad_data[0] = (eye == 0 ? 0.0f : 0.5f); screen_quad_data[1] = 1.0f;
		screen_quad_data[2] = (eye == 0 ? 0.0f : 0.5f); screen_quad_data[3] = 0.0f;
		screen_quad_data[4] = (eye == 0 ? 0.5f : 1.0f); screen_quad_data[5] = 1.0f;
		screen_quad_data[6] = (eye == 0 ? 0.5f : 1.0f); screen_quad_data[7] = 0.0f;
		screen_quad_uv_vbo->AssignData();
		
		if(ssao)
		{
			ssao->Render(0.5f, eye == 1 ? 0.5f : 0.0f); // binds its own fbo
			//glBindFramebuffer(GL_FRAMEBUFFER, fbo); // rebind fbo
		}

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glViewport(eye == 1 ? width[0] : 0, 0, width[eye], height[eye]);
		//glViewport(0, 0, screen_width, screen_height);
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
			fog_shader->SetCameraPosition(current_rendering_camera->GetPosition());

			RenderScreenQuad();

			current_read_color_tex = 1 - current_read_color_tex;
		}

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glBindFramebuffer(GL_FRAMEBUFFER, dst_fbo);

		glViewport(eye == 1 ? width[0] : 0, 0, width[eye], height[eye]);
		//glViewport(0, 0, screen_width, screen_height);

		post_process_shader->Bind();
		post_process_shader->SetFXAA(fxaa_enabled);
		post_process_shader->SetTextures(color_tex[current_read_color_tex], screen_width, screen_height);

		RenderScreenQuad();

		tShader::Unbind();

		if(dst_fbo != 0)
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


}
