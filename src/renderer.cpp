
#include "towerengine.h"

tRenderer::tRenderer(int width, int height, tWorld *world)
{
	geometry_pass_shader = new tGeometryPassShader();
	geometry_pass_shader->Init();
	//SetCurrentFaceShader(geometry_pass_shader);

	this->screen_width = width;
	this->screen_height = height;

	camera = new tCamera();
	camera_render_space = new tRenderSpace();

	glGenFramebuffers(1, &fbo);

	gbuffer = new tGBuffer(screen_width, screen_height, fbo, 2);

	directional_lighting_shader = new tDirectionalLightingShader();
	directional_lighting_shader->Init(gbuffer);

	point_lighting_shader = new tPointLightingShader();
	point_lighting_shader->Init(gbuffer);

	ambient_lighting_shader = new tAmbientLightingShader();
	ambient_lighting_shader->Init(gbuffer);

	cube_map_reflection_shader = new tCubeMapReflectionShader();
	cube_map_reflection_shader->Init(gbuffer);

	skybox_shader = new tSkyBoxShader();
	skybox_shader->Init();

	point_shadow_shader = new tPointShadowShader();
	point_shadow_shader->Init();

	point_shadow_blur_shader = new tPointShadowBlurShader();
	point_shadow_blur_shader->Init();

	directional_shadow_shader = new tDirectionalShadowShader();
	directional_shadow_shader->Init();

	directional_shadow_blur_shader = new tDirectionalShadowBlurShader();
	directional_shadow_blur_shader->Init();

	color_shader = new tColorShader();
	color_shader->Init();

	post_process_shader = new tPostProcessShader();
	post_process_shader->Init();

	this->world = world;

	ssao = 0;
	ssao_lighting_shader = 0;
	ssao_shader = 0;

	fog_shader = 0;
	fog_enabled = false;

	fxaa_enabled = false;

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	color_tex = new GLuint[2];
	glGenTextures(2, color_tex);

	glBindTexture(GL_TEXTURE_2D, color_tex[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex[0], 0);

	glBindTexture(GL_TEXTURE_2D, color_tex[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, color_tex[1], 0);

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


	screen_quad_vao = new tVAO();
	screen_quad_vbo = new tVBO<float>(2, screen_quad_vao, 4);

	float *screen_quad_data = screen_quad_vbo->GetData();
	screen_quad_data[0] = 0.0; screen_quad_data[1] = 1.0;
	screen_quad_data[2] = 0.0; screen_quad_data[3] = 0.0;
	screen_quad_data[4] = 1.0; screen_quad_data[5] = 0.0;
	screen_quad_data[6] = 1.0; screen_quad_data[7] = 1.0;

	screen_quad_vbo->AssignData();

	point_light_shadow_limit = -1;

	test_reflection = new tCubeMapReflection(256, Vec(15.78722, 1.31274, 11.01917));

}

tRenderer::~tRenderer(void)
{
	delete ssao;
	delete gbuffer;

	delete geometry_pass_shader;
	delete ambient_lighting_shader;
	delete directional_lighting_shader;
	delete point_lighting_shader;
	delete ssao_lighting_shader;
	delete ssao_shader;

	delete skybox_shader;
	delete point_shadow_shader;
	delete point_shadow_blur_shader;
	delete directional_shadow_shader;
	delete directional_shadow_blur_shader;
	delete color_shader;
	delete post_process_shader;
	delete fog_shader;

	delete camera;
	delete camera_render_space;

	delete test_reflection;
}

void tRenderer::InitSSAO(int kernel_size, float radius, int noise_tex_size)
{
	if(!ssao_lighting_shader)
	{
		ssao_lighting_shader = new tSSAOLightingShader();
		ssao_lighting_shader->Init(gbuffer);
	}

	if(!ssao_shader)
	{
		ssao_shader = new tSSAOShader();
		ssao_shader->Init();
	}

	delete ssao;
	ssao = new tSSAO(this, kernel_size, radius, noise_tex_size);
}

void tRenderer::SetFog(bool enabled, float start_dist, float end_dist, float exp, tVector color)
{
	fog_enabled = enabled;
	fog_start = start_dist;
	fog_end = end_dist;
	fog_exp = exp;

	if(!fog_enabled)
		return;

	if(!fog_shader)
	{
		fog_shader = new tFogShader();
		fog_shader->Init();
	}

	fog_shader->Bind();
	fog_shader->SetFog(start_dist, end_dist, exp, color);
}

void tRenderer::Render(GLuint dst_fbo)
{
	test_reflection->Render(this);

	camera->SetAspect((float)screen_width / (float)screen_height);

	world->FillRenderSpace(camera_render_space, camera);

	RenderShadowMaps();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GeometryPass();


	if(ssao)
		ssao->Render(); // binds its own fbo

	glBindFramebuffer(GL_FRAMEBUFFER, fbo); // rebind fbo
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	LightPass();

	ForwardPass();


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, 0.1, 2.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0); //pos x, y, z, to x, y, z

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	int current_read_color_tex = 0;

	if(fog_enabled)
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + (1-current_read_color_tex));

		fog_shader->Bind();
		fog_shader->SetTextures(gbuffer->GetTexture(tGBuffer::POSITION_TEX), color_tex[current_read_color_tex]);
		fog_shader->SetCameraPosition(camera->GetPosition());

		RenderScreenQuad();

		current_read_color_tex = 1 - current_read_color_tex;
	}


	glBindFramebuffer(GL_FRAMEBUFFER, dst_fbo);

	post_process_shader->Bind();
	post_process_shader->SetFXAA(fxaa_enabled);
	post_process_shader->SetTextures(color_tex[current_read_color_tex], screen_width, screen_height);

	RenderScreenQuad();

	tShader::Unbind();


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}




bool CompareFloatComparable(tComparable<float> *a, tComparable<float> *b)
{
	return a->GetCompareValue() < b->GetCompareValue();
}

void tRenderer::RenderShadowMaps(void)
{
	// fill render spaces

	render_point_light_shadows.clear();

	set<tPointLight *>::iterator pi;
	set<tDirectionalLight *>::iterator di;
	list<tPointLight *>::iterator pli;

	for(pi=camera_render_space->point_lights.begin(); pi!=camera_render_space->point_lights.end(); pi++)
	{
		if(!(*pi)->GetShadowEnabled())
			continue;

		render_point_light_shadows.push_back(*pi);
		(*pi)->SetCompareValue(((*pi)->GetPosition() - camera->GetPosition()).SquaredLen());
	}


	if(point_light_shadow_limit == 0)
		render_point_light_shadows.clear();
	else if(point_light_shadow_limit > 0)
	{
		render_point_light_shadows.sort(CompareFloatComparable);

		while((int)render_point_light_shadows.size() > point_light_shadow_limit)
			render_point_light_shadows.pop_back();
	} // < 0 renders all point lights

	// add all point light shadows which have not been rendered at all yet
	for(pi=camera_render_space->point_lights.begin(); pi!=camera_render_space->point_lights.end(); pi++)
	{
		if(!(*pi)->GetShadowInvalid())
			continue;
		render_point_light_shadows.push_back(*pi);
	}
	render_point_light_shadows.unique();



	for(pli=render_point_light_shadows.begin(); pli!=render_point_light_shadows.end(); pli++)
		world->FillRenderObjectSpace((*pli)->GetShadow()->GetRenderObjectSpace(), *pli);



	// finally render shadows

	for(pli=render_point_light_shadows.begin(); pli!=render_point_light_shadows.end(); pli++)
		(*pli)->RenderShadow(this);


	for(di=camera_render_space->dir_lights.begin(); di!=camera_render_space->dir_lights.end(); di++)
		(*di)->RenderShadow(camera, this);
}


void tRenderer::GeometryPass(void)
{
	gbuffer->BindDrawBuffers();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	camera->SetModelViewProjectionMatrix();

	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	SetCurrentFaceShader(geometry_pass_shader);
	BindCurrentFaceShader();

	camera_render_space->GeometryPass(this);
}

void tRenderer::ReflectionPass(void)
{
	camera->SetModelViewProjectionMatrix();

	cube_map_reflection_shader->Bind();
	cube_map_reflection_shader->SetCameraPosition(camera->GetPosition());
	cube_map_reflection_shader->SetCubeMapTexture(test_reflection->GetCubeMapTexture());

	set<tObject *>::iterator i;
	for(i=camera_render_space->objects.begin(); i!=camera_render_space->objects.end(); i++)
	{
		if((*i)->GetCubeMapReflectionEnabled())
			(*i)->CubeMapReflectionPass(this);
	}
}


void tRenderer::LightPass(void)
{
	tSkyBox *sky_box = world->GetSkyBox();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	camera->SetModelViewProjectionMatrix();

	if(sky_box)
		sky_box->Paint(this, camera->GetPosition());


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();



	gbuffer->BindTextures();

	if(ssao)
	{
		ssao_lighting_shader->Bind();
		ssao_lighting_shader->SetSSAOTexture(ssao->GetSSAOTexture());
		ssao_lighting_shader->SetAmbientLight(world->GetAmbientColor());
	}
	else
	{
		ambient_lighting_shader->Bind();
		ambient_lighting_shader->SetAmbientLight(world->GetAmbientColor());
	}

	RenderScreenQuad();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	ReflectionPass();


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	directional_lighting_shader->Bind();
	directional_lighting_shader->SetCameraPosition(camera->GetPosition());

	set<tDirectionalLight *>::iterator dir_light_it;
	for(dir_light_it=camera_render_space->dir_lights.begin(); dir_light_it!=camera_render_space->dir_lights.end(); dir_light_it++)
	{
		(*dir_light_it)->InitRenderLighting(directional_lighting_shader);
		RenderScreenQuad();
	}


	point_lighting_shader->Bind();
	point_lighting_shader->SetCameraPosition(camera->GetPosition());

	set<tPointLight *>::iterator point_light_it;
	for(point_light_it=camera_render_space->point_lights.begin(); point_light_it!=camera_render_space->point_lights.end(); point_light_it++)
	{
		tPointLight *light = *point_light_it;
		if(!light->GetEnabled())
			continue;
		light->InitRenderLighting(point_lighting_shader);
		RenderScreenQuad();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void tRenderer::ForwardPass(void)
{
	camera->SetModelViewProjectionMatrix();
	camera_render_space->ForwardPass(this);
}


void tRenderer::ChangeSize(int width, int height)
{
	if(screen_width == width && screen_height == height)
		return;

	screen_width = width;
	screen_height = height;

	glBindTexture(GL_TEXTURE_2D, color_tex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, color_tex[1]);
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


















