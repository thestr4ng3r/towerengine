
#include "towerengine.h"

CRenderer::CRenderer(int width, int height, CWorld *world)
{
	this->screen_width = width;
	this->screen_height = height;

	this->world = world;

	gbuffer = new CGBuffer(screen_width, screen_height);

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

	glGenRenderbuffers(1, &depth_rbo);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rbo);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, screen_width, screen_height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	point_light_count = 0;
	point_light_pos = new float[CLightPassShader::max_point_lights * 3];
	point_light_color = new float[CLightPassShader::max_point_lights * 3];
	point_light_distance = new float[CLightPassShader::max_point_lights];
	point_light_shadow_enabled = new int[CLightPassShader::max_point_lights];
	point_light_shadow_maps = new GLuint[CLightPassShader::max_point_lights];

	dir_light_count = 0;
	dir_light_dir = new float[CLightPassShader::max_directional_lights * 3];
	dir_light_color = new float[CLightPassShader::max_directional_lights * 3];
	dir_light_shadow_enabled = new int[CLightPassShader::max_directional_lights];
	dir_light_shadow_clip = new float[CLightPassShader::max_directional_lights * 2];
	dir_light_shadow_tex_matrix = new float[CLightPassShader::max_directional_lights * 16 * CLightPassShader::max_directional_light_splits];
	dir_light_shadow_splits_count = new float[CLightPassShader::max_directional_lights];
	dir_light_shadow_splits_z = new float[CLightPassShader::max_directional_lights * (CLightPassShader::max_directional_light_splits+1)];
	dir_light_shadow_maps = new GLuint[CLightPassShader::max_directional_lights];
}

CRenderer::~CRenderer(void)
{
	delete[] point_light_pos;
	delete[] point_light_color;
	delete[] point_light_distance;
	delete[] point_light_shadow_enabled;
	delete[] point_light_shadow_maps;

	delete[] dir_light_dir;
	delete[] dir_light_color;
	delete[] dir_light_shadow_enabled;
	delete[] dir_light_shadow_clip;
	delete[] dir_light_shadow_tex_matrix;
	delete[] dir_light_shadow_splits_count;
	delete[] dir_light_shadow_splits_z;
	delete[] dir_light_shadow_maps;
}

void CRenderer::Render(void)
{
	CCamera *camera = world->GetCamera();

	camera->SetAspect((float)screen_width / (float)screen_height);

	world->FillRenderSpaces();

	world->RenderShadowMaps();

	GeometryPass();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	LightPass();
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

	CEngine::GetPostProcessShader()->Bind();
	CEngine::GetPostProcessShader()->SetTextures(color_tex, gbuffer->GetDepthTexture(), screen_width, screen_height);

	glBegin(GL_QUADS);
	glVertex2f(0.0, 1.0);
	glVertex2f(0.0, 0.0);
	glVertex2f(1.0, 0.0);
	glVertex2f(1.0, 1.0);
	glEnd();

	CShader::Unbind();
}


void CRenderer::GeometryPass(void)
{
	CCamera *camera = world->GetCamera();

	gbuffer->BindForDrawing();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	camera->SetModelviewProjectionMatrix();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	CEngine::SetCurrentFaceShader(CEngine::GetGeometryPassShader());
	CEngine::BindCurrentFaceShader();

	world->GetCameraRenderSpace()->Render();

	CShader::Unbind();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void CRenderer::LightPass(void)
{
	CCamera *camera = world->GetCamera();
	CSkyBox *sky_box = world->GetSkyBox();

	world->GetPointLightUniforms(point_light_count, point_light_pos, point_light_color, point_light_distance, point_light_shadow_enabled, point_light_shadow_maps);

	world->GetDirectionalLightUniforms(dir_light_count, dir_light_dir, dir_light_color, dir_light_shadow_clip, dir_light_shadow_tex_matrix, dir_light_shadow_splits_count,
										dir_light_shadow_splits_z, dir_light_shadow_enabled, dir_light_shadow_maps);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

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
	glEnable(GL_BLEND);

	CEngine::GetLightPassShader()->Bind();
	CEngine::GetLightPassShader()->SetGBuffer(gbuffer);
	CEngine::GetLightPassShader()->SetPointLights(		point_light_count,
														point_light_pos,
														point_light_color,
														point_light_distance,
														point_light_shadow_enabled,
														point_light_shadow_maps);
	CEngine::GetLightPassShader()->SetDirectionalLights(	dir_light_count,
															dir_light_dir,
															dir_light_color,
															dir_light_shadow_enabled,
															dir_light_shadow_maps,
															dir_light_shadow_clip,
															dir_light_shadow_tex_matrix,
															dir_light_shadow_splits_count,
															dir_light_shadow_splits_z);
	CEngine::GetLightPassShader()->SetLightAmbientColor(world->GetAmbientColor());
	CEngine::GetLightPassShader()->SetCameraPosition(camera->GetPosition());


	glBegin(GL_QUADS);
	glVertex2f(0.0, 1.0);
	glVertex2f(0.0, 0.0);
	glVertex2f(1.0, 0.0);
	glVertex2f(1.0, 1.0);
	glEnd();

	CShader::Unbind();
}
