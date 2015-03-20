
#include "towerengine.h"

tRenderer::tRenderer(int width, int height, tWorld *world)
{
	this->screen_width = width;
	this->screen_height = height;

	this->world = world;

	ssao = 0;

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screen_width, screen_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

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
	delete ssao;
	delete gbuffer;
}

void tRenderer::InitSSAO(int kernel_size, float radius, int noise_tex_size)
{
	delete ssao;
	ssao = new tSSAO(this, kernel_size, radius, noise_tex_size);
}

void tRenderer::Render(GLuint dst_fbo)
{
	tCamera *camera = world->GetCamera();

	camera->SetAspect((float)screen_width / (float)screen_height);

	world->FillRenderSpaces();

	world->RenderShadowMaps();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GeometryPass();

	if(ssao)
		ssao->Render(); // binds its own fbo

	glBindFramebuffer(GL_FRAMEBUFFER, fbo); // rebind fbo
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	LightPass();

	ForwardPass();

	glBindFramebuffer(GL_FRAMEBUFFER, dst_fbo);


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


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	glOrtho(0.0, 1.0, 0.0, 1.0, 0.1, 2.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0); //pos x, y, z, to x, y, z

	glDisable(GL_DEPTH_TEST);


	if(ssao)
	{
		tEngine::GetSSAOLightingShader()->Bind();
		tEngine::GetSSAOLightingShader()->SetGBuffer(gbuffer);
		tEngine::GetSSAOLightingShader()->SetSSAOTexture(ssao->GetSSAOTexture());
		tEngine::GetAmbientLightingShader()->SetAmbientLight(world->GetAmbientColor());
	}
	else
	{
		tEngine::GetAmbientLightingShader()->Bind();
		tEngine::GetAmbientLightingShader()->SetGBuffer(gbuffer);
		tEngine::GetAmbientLightingShader()->SetAmbientLight(world->GetAmbientColor());
	}

	RenderScreenQuad();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);


	tEngine::GetDirectionalLightingShader()->Bind();
	tEngine::GetDirectionalLightingShader()->SetGBuffer(gbuffer);
	tEngine::GetDirectionalLightingShader()->SetCameraPosition(camera->GetPosition());

	for(i=0; i<world->GetDirectionalLightsCount(); i++)
	{
		world->GetDirectionalLight(i)->InitRenderLighting();
		RenderScreenQuad();
	}


	tEngine::GetPointLightingShader()->Bind();
	tEngine::GetPointLightingShader()->SetGBuffer(gbuffer);
	tEngine::GetPointLightingShader()->SetCameraPosition(camera->GetPosition());

	for(i=0; i<world->GetPointLightsCount(); i++)
	{
		world->GetPointLight(i)->InitRenderLighting();
		RenderScreenQuad();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void tRenderer::ForwardPass(void)
{
	world->GetCamera()->SetModelviewProjectionMatrix();

	world->GetCameraRenderSpace()->ForwardPass();
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

	if(ssao)
		ssao->ChangeScreenSize(width, height);

	glBindTexture(GL_TEXTURE_2D, 0);
}


void tRenderer::RenderScreenQuad(void)
{
	screen_quad_vao->Bind();
	screen_quad_vbo->SetAttribute(tShader::vertex_attribute, GL_FLOAT);
	screen_quad_vao->Draw(GL_QUADS, 0, 4);
}


















