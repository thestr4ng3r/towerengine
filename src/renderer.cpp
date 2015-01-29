
#include "towerengine.h"

tRenderer::tRenderer(int width, int height, tWorld *world)
{
	this->screen_width = width;
	this->screen_height = height;

	this->world = world;

	ssao.kernel = 0;
	ssao.kernel_size = 0;
	ssao.noise_tex = 0;
	ssao.noise_tex_size = 0;
	ssao.enabled = false;
	ssao.tex = 0;
	ssao.radius = 0.0;

	fxaa_enabled = false;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &color_tex);

	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);

	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	gbuffer = new tGBuffer(screen_width, screen_height, fbo, 1);

	screen_quad_vao = new tVAO();
	screen_quad_vbo = new tVBO<float>(2, screen_quad_vao, 4);

	float *screen_quad_data = screen_quad_vbo->GetData();
	screen_quad_data[0] = 0.0; screen_quad_data[1] = 1.0;
	screen_quad_data[2] = 0.0; screen_quad_data[3] = 0.0;
	screen_quad_data[4] = 1.0; screen_quad_data[5] = 0.0;
	screen_quad_data[6] = 1.0; screen_quad_data[7] = 1.0;

	screen_quad_vbo->AssignData();

}

tRenderer::~tRenderer(void)
{
	delete gbuffer;
}

void tRenderer::InitSSAO(int kernel_size, float radius, int noise_tex_size)
{
	ssao.kernel = new float[kernel_size*3];
	ssao.kernel_size = kernel_size;

	ssao.radius = radius;

	float scale;
	tVector v;

	for(int i=0; i<ssao.kernel_size; i++)
	{
		v.x = RandomFloat(-1.0, 1.0);
		v.y = RandomFloat(-1.0, 1.0);
		v.z = RandomFloat(0.0, 1.0);
		v.Normalize();

		scale = (float)i / (float)ssao.kernel_size;
		scale = Mix(0.1, 1.0, scale * scale);
		v *= scale;

		memcpy(ssao.kernel + i*3, v.v, sizeof(float) * 3);
	}

	ssao.noise_tex_size = noise_tex_size;
	GLubyte *noise_tex_data = new GLubyte[noise_tex_size * noise_tex_size * 3];
	int x, y;

	for(y=0; y<ssao.noise_tex_size; y++)
	{
		for(x=0; x<ssao.noise_tex_size; x++)
		{
			noise_tex_data[y*ssao.noise_tex_size*3 + x*3 + 0] = (GLubyte)(RandomFloat() * 255.0);
			noise_tex_data[y*ssao.noise_tex_size*3 + x*3 + 1] = (GLubyte)(RandomFloat() * 255.0);
			noise_tex_data[y*ssao.noise_tex_size*3 + x*3 + 2] = 0.0;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &ssao.noise_tex);
	glBindTexture(GL_TEXTURE_2D, ssao.noise_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ssao.noise_tex_size, ssao.noise_tex_size, 0, GL_RGB, GL_UNSIGNED_BYTE, noise_tex_data);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &ssao.tex);

	glBindTexture(GL_TEXTURE_2D, ssao.tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screen_width, screen_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	ssao.draw_buffer = GL_COLOR_ATTACHMENT0 + 1 + gbuffer->GetTexCount();
	glFramebufferTexture2D(GL_FRAMEBUFFER, ssao.draw_buffer, GL_TEXTURE_2D, ssao.tex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ssao.enabled = true;
}

void tRenderer::Render(void)
{
	tCamera *camera = world->GetCamera();

	camera->SetAspect((float)screen_width / (float)screen_height);

	world->FillRenderSpaces();

	world->RenderShadowMaps();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GeometryPass();

	if(ssao.enabled)
		RenderSSAO();

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	LightPass();

	ForwardPass();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, 0.1, 2.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0); //pos x, y, z, to x, y, z

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	tEngine::GetPostProcessShader()->Bind();
	tEngine::GetPostProcessShader()->SetFXAA(fxaa_enabled);
	tEngine::GetPostProcessShader()->SetTextures(color_tex, depth_tex, screen_width, screen_height);

	glBegin(GL_QUADS);
	glVertex2f(0.0, 1.0);
	glVertex2f(0.0, 0.0);
	glVertex2f(1.0, 0.0);
	glVertex2f(1.0, 1.0);
	glEnd();

	tShader::Unbind();
}


void tRenderer::GeometryPass(void)
{
	tCamera *camera = world->GetCamera();

	gbuffer->Bind();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	camera->SetModelviewProjectionMatrix();

	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	tEngine::SetCurrentFaceShader(tEngine::GetGeometryPassShader());
	tEngine::BindCurrentFaceShader();

	world->GetCameraRenderSpace()->GeometryPass();
}

void tRenderer::LightPass(void)
{
	int i;

	tCamera *camera = world->GetCamera();
	tSkyBox *sky_box = world->GetSkyBox();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	glDisable(GL_BLEND);

	camera->SetModelviewProjectionMatrix();

	if(sky_box)
		sky_box->Paint(camera->GetPosition());


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(camera->GetAngle(), camera->GetAspect(), camera->GetNearClip(), camera->GetFarClip());
	glOrtho(0.0, 1.0, 0.0, 1.0, 0.1, 2.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0); //pos x, y, z, to x, y, z

	glDisable(GL_DEPTH_TEST);

	tEngine::GetAmbientLightingShader()->Bind();
	tEngine::GetAmbientLightingShader()->SetGBuffer(gbuffer);
	tEngine::GetAmbientLightingShader()->SetAmbientLight(world->GetAmbientColor());

	RenderLightingScreenQuad();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);


	tEngine::GetDirectionalLightingShader()->Bind();
	tEngine::GetDirectionalLightingShader()->SetGBuffer(gbuffer);
	tEngine::GetDirectionalLightingShader()->SetCameraPosition(camera->GetPosition());

	for(i=0; i<world->GetDirectionalLightsCount(); i++)
	{
		world->GetDirectionalLight(i)->InitRenderLighting();
		RenderLightingScreenQuad();
	}


	tEngine::GetPointLightingShader()->Bind();
	tEngine::GetPointLightingShader()->SetGBuffer(gbuffer);
	tEngine::GetPointLightingShader()->SetCameraPosition(camera->GetPosition());

	for(i=0; i<world->GetPointLightsCount(); i++)
	{
		world->GetPointLight(i)->InitRenderLighting();
		RenderLightingScreenQuad();
	}

	if(ssao.enabled)
	{
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);

		tEngine::GetSSAOLightingShader()->Bind();
		tEngine::GetSSAOLightingShader()->SetGBuffer(gbuffer);
		tEngine::GetSSAOLightingShader()->SetSSAOTexture(ssao.tex);

		RenderLightingScreenQuad();
	}



	/*tEngine::GetLightPassShader()->Bind();
	tEngine::GetLightPassShader()->SetGBuffer(gbuffer);
	tEngine::GetLightPassShader()->SetPointLights(		point_light_count,
														point_light_pos,
														point_light_color,
														point_light_distance,
														point_light_shadow_enabled,
														point_light_shadow_maps);
	tEngine::GetLightPassShader()->SetDirectionalLights(	dir_light_count,
															dir_light_dir,
															dir_light_color,
															dir_light_shadow_enabled,
															dir_light_shadow_maps,
															dir_light_shadow_clip,
															dir_light_shadow_tex_matrix,
															dir_light_shadow_splits_count,
															dir_light_shadow_splits_z);
	tEngine::GetLightPassShader()->SetLightAmbientColor(world->GetAmbientColor());
	tEngine::GetLightPassShader()->SetCameraPosition(camera->GetPosition());
	tEngine::GetLightPassShader()->SetSSAO(ssao.enabled, ssao.tex);*/




	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void tRenderer::ForwardPass(void)
{
	world->GetCamera()->SetModelviewProjectionMatrix();

	world->GetCameraRenderSpace()->ForwardPass();
}

void tRenderer::RenderSSAO(void)
{
	tSSAOShader *ssao_shader = tEngine::GetSSAOShader();
	tVector2 noise_tex_scale;
	tVector *view_rays = world->GetCamera()->GetViewRays();

	noise_tex_scale.x = (float)screen_width / (float)ssao.noise_tex_size;
	noise_tex_scale.y = (float)screen_height / (float)ssao.noise_tex_size;

	glDrawBuffer(ssao.draw_buffer);

	ssao_shader->Bind();
	ssao_shader->SetKernel(ssao.kernel_size, ssao.kernel);
	ssao_shader->SetNoiseTex(ssao.noise_tex, noise_tex_scale);
	ssao_shader->SetTextures(depth_tex, gbuffer->GetTexture(tGBuffer::POSITION_TEX), gbuffer->GetTexture(tGBuffer::NORMAL_TEX));
	ssao_shader->SetMatrices(projection_matrix, modelview_matrix);
	ssao_shader->SetRadius(ssao.radius);
	ssao_shader->SetCamera(world->GetCamera()->GetPosition(), world->GetCamera()->GetDirection());

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, 0.1, 2.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0); //pos x, y, z, to x, y, z

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glBegin(GL_QUADS);
	view_rays[0].AttrToGL(tSSAOShader::view_ray_attribute);
	glVertex2f(0.0, 1.0);
	view_rays[1].AttrToGL(tSSAOShader::view_ray_attribute);
	glVertex2f(0.0, 0.0);
	view_rays[2].AttrToGL(tSSAOShader::view_ray_attribute);
	glVertex2f(1.0, 0.0);
	view_rays[3].AttrToGL(tSSAOShader::view_ray_attribute);
	glVertex2f(1.0, 1.0);
	glEnd();
}

void tRenderer::ChangeSize(int width, int height)
{
	if(screen_width == width && screen_height == height)
		return;

	screen_width = width;
	screen_height = height;

	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	gbuffer->ChangeSize(width, height);

	if(ssao.tex)
	{
		glBindTexture(GL_TEXTURE_2D, ssao.tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screen_width, screen_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}


void tRenderer::RenderLightingScreenQuad(void)
{
	screen_quad_vao->Bind();
	screen_quad_vbo->SetAttribute(tLightingShader::vertex_attribute, GL_FLOAT);
	screen_quad_vao->Draw(GL_QUADS, 0, 4);
}


















