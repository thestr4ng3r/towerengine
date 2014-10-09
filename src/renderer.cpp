
#include "towerengine.h"

CRenderer::CRenderer(int width, int height, CWorld *world)
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

	gbuffer = new CGBuffer(screen_width, screen_height, fbo, 1);

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

void CRenderer::InitSSAO(int kernel_size, float radius, int noise_tex_size)
{
	ssao.kernel = new float[kernel_size*3];
	ssao.kernel_size = kernel_size;

	ssao.radius = radius;

	float scale;
	CVector v;

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

void CRenderer::Render(void)
{
	CCamera *camera = world->GetCamera();

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

	CEngine::GetPostProcessShader()->Bind();
	CEngine::GetPostProcessShader()->SetTextures(color_tex, depth_tex, screen_width, screen_height);

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

	gbuffer->Bind();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	camera->SetModelviewProjectionMatrix();

	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	CEngine::SetCurrentFaceShader(CEngine::GetGeometryPassShader());
	CEngine::BindCurrentFaceShader();

	world->GetCameraRenderSpace()->GeometryPass();
}

void CRenderer::LightPass(void)
{
	CCamera *camera = world->GetCamera();
	CSkyBox *sky_box = world->GetSkyBox();

	world->GetPointLightUniforms(point_light_count, point_light_pos, point_light_color, point_light_distance, point_light_shadow_enabled, point_light_shadow_maps);

	world->GetDirectionalLightUniforms(dir_light_count, dir_light_dir, dir_light_color, dir_light_shadow_clip, dir_light_shadow_tex_matrix, dir_light_shadow_splits_count,
										dir_light_shadow_splits_z, dir_light_shadow_enabled, dir_light_shadow_maps);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
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
	CEngine::GetLightPassShader()->SetSSAO(ssao.enabled, ssao.tex);


	glBegin(GL_QUADS);
	glVertex2f(0.0, 1.0);
	glVertex2f(0.0, 0.0);
	glVertex2f(1.0, 0.0);
	glVertex2f(1.0, 1.0);
	glEnd();
}

void CRenderer::ForwardPass(void)
{
	world->GetCamera()->SetModelviewProjectionMatrix();

	world->GetCameraRenderSpace()->ForwardPass();
}

void CRenderer::RenderSSAO(void)
{
	CSSAOShader *ssao_shader = CEngine::GetSSAOShader();
	CVector2 noise_tex_scale;
	CVector *view_rays = world->GetCamera()->GetViewRays();

	noise_tex_scale.x = (float)screen_width / (float)ssao.noise_tex_size;
	noise_tex_scale.y = (float)screen_height / (float)ssao.noise_tex_size;

	glDrawBuffer(ssao.draw_buffer);

	ssao_shader->Bind();
	ssao_shader->SetKernel(ssao.kernel_size, ssao.kernel);
	ssao_shader->SetNoiseTex(ssao.noise_tex, noise_tex_scale);
	ssao_shader->SetTextures(depth_tex, gbuffer->GetTexture(CGBuffer::POSITION_TEX), gbuffer->GetTexture(CGBuffer::NORMAL_TEX));
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
	view_rays[0].AttrToGL(CSSAOShader::view_ray_attribute);
	glVertex2f(0.0, 1.0);
	view_rays[1].AttrToGL(CSSAOShader::view_ray_attribute);
	glVertex2f(0.0, 0.0);
	view_rays[2].AttrToGL(CSSAOShader::view_ray_attribute);
	glVertex2f(1.0, 0.0);
	view_rays[3].AttrToGL(CSSAOShader::view_ray_attribute);
	glVertex2f(1.0, 1.0);
	glEnd();
}

void CRenderer::ChangeSize(int width, int height)
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




















