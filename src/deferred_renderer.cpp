
#include "towerengine.h"

using namespace std;






void tDeferredRenderer::InitDeferredRenderer(int width, int height, tWorld *world, bool bindless_textures)
{
	InitRenderer(world);

	this->screen_width = width;
	this->screen_height = height;

	this->bindless_textures_enabled = tEngine::GetARBBindlessTextureSupported() && bindless_textures;


	glGenFramebuffers(1, &fbo);
	gbuffer = new tGBuffer(screen_width, screen_height, fbo, 2);

	InitShaders();

	ssao = 0;
	ssao_ambient_lighting_shader = 0;
	ssao_lighting_shader = 0;
	ssao_shader = 0;
	ssao_ambient_only = false;

	fog_shader = 0;
	fog_enabled = false;

	fxaa_enabled = false;

	depth_prepass_enabled = true;

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


	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	point_lights_buffer = 0;
#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
	if(bindless_textures_enabled)
	{
		point_lights_buffer = new tLightingShaderPointLightsBuffer();
	}
#endif

	matrix_buffer = new tMatrixBuffer();
	position_restore_data_buffer = new tPositionRestoreDataBuffer();



	current_read_color_tex = 0;
}

void tDeferredRenderer::InitShaders(void)
{
	geometry_pass_shader = new tGeometryPassShader();
	directional_lighting_shader = new tDirectionalLightingShader(gbuffer);

	//if(!bindless_textures_enabled)
	//{
	// VERY IMPORTANT:	these shaders must be ordered descending by lights_count.
	//					also, there has to be one shader with a lights_count of 1
	point_lighting_shaders.push_back(new tPointLightingShader(8, gbuffer));
	point_lighting_shaders.push_back(new tPointLightingShader(7, gbuffer));
	point_lighting_shaders.push_back(new tPointLightingShader(6, gbuffer));
	point_lighting_shaders.push_back(new tPointLightingShader(5, gbuffer));
	point_lighting_shaders.push_back(new tPointLightingShader(4, gbuffer));
	point_lighting_shaders.push_back(new tPointLightingShader(3, gbuffer));
	point_lighting_shaders.push_back(new tPointLightingShader(2, gbuffer));
	point_lighting_shaders.push_back(new tPointLightingShader(1, gbuffer));
	//}

	simple_forward_shader = new tSimpleForwardShader();
	refraction_shader = new tRefractionShader();

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
	if(bindless_textures_enabled)
	{
		lighting_shader = new tLightingShader(gbuffer);

		if(!lighting_shader->GetStatus())
		{
			delete lighting_shader;
			lighting_shader = 0;
			bindless_textures_enabled = false;
			printf("failed to compile lighting shader. using fallback lighting.\n");
		}
	}
	else
		lighting_shader = 0;
#else
	lighting_shader = 0;
#endif

	///if(!bindless_textures_enabled)
	//{
	ambient_lighting_shader = new tAmbientLightingShader(gbuffer, false);
	//}

	skybox_shader = new tSkyBoxShader();
	color_shader = new tColorShader();
	post_process_shader = new tPostProcessShader();
	particle_forward_shader = new tParticleForwardShader(gbuffer);
	particle_deferred_shader = new tParticleDeferredShader(gbuffer);
	cube_map_blur_shader = new tCubeMapBlurShader();
}

tDeferredRenderer::~tDeferredRenderer(void)
{
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(2, color_tex);
	delete [] color_tex;


	delete ssao;
	delete gbuffer;

	delete geometry_pass_shader;
	delete simple_forward_shader;
	delete refraction_shader;
	delete ambient_lighting_shader;
	delete directional_lighting_shader;
	delete depth_pass_shader;
	delete lighting_shader;
	delete cube_map_blur_shader;

	for(vector<tPointLightingShader *>::iterator psi=point_lighting_shaders.begin(); psi!=point_lighting_shaders.end(); psi++)
		delete *psi;

	delete ssao_lighting_shader;
	delete ssao_ambient_lighting_shader;
	delete ssao_shader;

	delete skybox_shader;
	delete color_shader;
	delete post_process_shader;
	delete fog_shader;

	delete particle_forward_shader;
	delete particle_deferred_shader;
	
	delete matrix_buffer;
	delete position_restore_data_buffer;
	delete point_lights_buffer;
}

void tDeferredRenderer::InitSSAO(bool ambient_only, int kernel_size, float radius, int noise_tex_size)
{
	ssao_ambient_only = ambient_only;

	if(ssao_ambient_only)
	{
		if(!ssao_ambient_lighting_shader && !bindless_textures_enabled)
		{
			ssao_ambient_lighting_shader = new tAmbientLightingShader(gbuffer, true);
		}
	}
	else
	{
		if(!ssao_lighting_shader)
		{
			ssao_lighting_shader = new tSSAOLightingShader(gbuffer);
		}
	}

	if(!ssao_shader)
	{
		ssao_shader = new tSSAOShader();
	}

	delete ssao;
	ssao = new tSSAO(this, kernel_size, radius, noise_tex_size);
}

void tDeferredRenderer::SetFog(bool enabled, float start_dist, float end_dist, float exp, tVector color)
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





void tDeferredRenderer::PrepareRender(tCamera *camera, tRenderSpace *render_space)
{
	tRenderer::PrepareRender(camera, render_space);
	RenderCubeMapReflections();
}

void tDeferredRenderer::RenderCubeMapReflections(void)
{
	shadow_pass = false;
	for(int i = 0; i<world->GetCubeMapReflectionsCount(); i++)
	{
		tCubeMapReflection *reflection = world->GetCubeMapReflection(i);
		if(reflection->GetInvalid())
			reflection->Render(this);
	}
}


void tDeferredRenderer::DepthPrePass(void)
{
	shadow_pass = false;

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	SetCurrentFaceShader(depth_pass_shader);
	BindCurrentFaceShader();

	current_rendering_render_space->DepthPrePass(this);
}


void tDeferredRenderer::GeometryPass(void)
{
	shadow_pass = false;

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if(depth_prepass_enabled)
	{
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_EQUAL);
	}
	else
	{
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);
	}

	glDisable(GL_BLEND);

	SetCurrentFaceShader(geometry_pass_shader);
	BindCurrentFaceShader();

	geometry_pass_shader->SetCameraPosition(current_rendering_camera->GetPosition());

	//tCubeMapReflection *cube_map_reflection = 0;
	//if(world->GetCubeMapReflectionsCount() > 0)
	//	cube_map_reflection = world->GetCubeMapReflection(0);

	//if (cube_map_reflection)
	//	geometry_pass_shader->SetCubeMapReflectionTexture(cube_map_reflection->GetCubeMapTexture());
	//else
	//	geometry_pass_shader->SetCubeMapReflectionTexture(0);

	current_rendering_render_space->GeometryPass(this);

	if(depth_prepass_enabled)
		glDepthFunc(GL_LESS);

	bool particle_rendering_initialized = false;
	for(int i=0; i<world->GetParticleSystemsCount(); i++)
	{
		tParticleSystem *particle_system = world->GetParticleSystem(i);

		if(particle_system->GetRenderType() != tParticleSystem::DEFERRED_LIT)
			continue;

		if(!particle_rendering_initialized)
		{
			particle_deferred_shader->Bind();
			particle_deferred_shader->SetModelViewProjectionMatrices(current_rendering_camera->GetModelViewMatrix().GetData(), current_rendering_camera->GetProjectionMatrix().GetData());
			particle_deferred_shader->SetFaceNormal(-current_rendering_camera->GetDirection());
			particle_rendering_initialized = true;
		}

		particle_system->Render(this, particle_deferred_shader);
	}

	if(particle_rendering_initialized || depth_prepass_enabled)
		glDepthMask(GL_TRUE);
}

void tDeferredRenderer::LightPass(void)
{
	shadow_pass = false;

	tSkyBox *sky_box = world->GetSkyBox();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	if(sky_box)
	{
		skybox_shader->Bind();
		skybox_shader->SetCameraPosition(current_rendering_camera->GetPosition());
		sky_box->Paint(this, current_rendering_camera->GetPosition());
	}

	gbuffer->BindTextures();

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
	if(bindless_textures_enabled)
		BindlessTexturesLightPass();
	else
#endif
		LegacyLightPass();

	// blending should be enabled and set to GL_ONE/GL_ONE from LightPass methods


	// directional lights
	directional_lighting_shader->Bind(); // TODO: only bind if there are any directional lights
	directional_lighting_shader->SetCameraPosition(current_rendering_camera->GetPosition());

	set<tDirectionalLight *>::iterator dir_light_it;
	for(dir_light_it=current_rendering_render_space->dir_lights.begin(); dir_light_it!=current_rendering_render_space->dir_lights.end(); dir_light_it++)
	{
		(*dir_light_it)->InitRenderLighting(directional_lighting_shader);
		RenderScreenQuad();
	}




	glEnable(GL_BLEND);
	if(ssao && !ssao_ambient_only)
	{
		glBlendFunc(GL_DST_COLOR, GL_ZERO);

		ssao_lighting_shader->Bind();
		ssao_lighting_shader->SetSSAOTexture(ssao->GetSSAOTexture());

		RenderScreenQuad();
	}



	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}



#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
void tDeferredRenderer::BindlessTexturesLightPass(void)
{
	// blending should be disabled here

	// point lights
	vector<tPointLight *> point_lights;

	set<tPointLight *>::iterator point_light_sit;
	for(point_light_sit=current_rendering_render_space->point_lights.begin(); point_light_sit!=current_rendering_render_space->point_lights.end(); point_light_sit++)
	{
		tPointLight *light = *point_light_sit;
		if(!light->GetEnabled())
			continue;
		point_lights.push_back(light);
	}

	point_lights_buffer->UpdateBuffer(point_lights);
	point_lights_buffer->Bind();



	lighting_shader->Bind();
	lighting_shader->SetCameraPosition(current_rendering_camera->GetPosition());
	lighting_shader->SetAmbientLight(world->GetAmbientColor());
	if(ssao && ssao_ambient_only)
		lighting_shader->SetSSAO(true, ssao->GetTextureHandle());
	else
		lighting_shader->SetSSAO(false, 0);


	SetReflections(lighting_shader, GetCurrentRenderingCamera()->GetPosition()); // TODO: For VR, a common position for both eyes has to be used


	//lighting_shader->SetPositionData(depth_tex);

	RenderScreenQuad();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
}
#endif


void tDeferredRenderer::SetReflections(tReflectingShader *shader, tVector pos)
{
	vector<tCubeMapReflection *> reflections;

	// TODO: Blend between Reflections
	if(world->GetCubeMapReflectionsCount() > 0)
	{
		for(int i=0; i<world->GetCubeMapReflectionsCount(); i++)
		{
			tCubeMapReflection *reflection = world->GetCubeMapReflection(i);
			tVector dir = pos - reflection->GetPosition();

			if(reflection->GetExtent().ContainsPoint(dir))
				reflections.push_back(reflection);
		}
	}

	if(reflections.size() >= 2)
	{
		// see https://seblagarde.wordpress.com/2012/09/29/image-based-lighting-approaches-and-parallax-corrected-cubemap/

		tCubeMapReflection *reflection1 = reflections[0];
		tCubeMapReflection *reflection2 = reflections[1];

		float influence1 = reflection1->GetExtent().GetNormalizedBoxDistanceToCenter(pos - reflection1->GetPosition());
		float influence2 = reflection2->GetExtent().GetNormalizedBoxDistanceToCenter(pos - reflection2->GetPosition());

		float sum = influence1 + influence2;
		float inv_sum = (1.0f - influence1) + (1.0f - influence2);

		float blend1 = 1.0f - (influence1 / sum);
		blend1 *= (1.0f - influence1) / inv_sum;

		float blend2 = 1.0f - (influence2 / sum);
		blend2 *= (1.0f - influence2) / inv_sum;

		float blend = blend1 / (blend1 + blend2);

		shader->SetReflectionTextures(reflections[0]->GetCubeMapTexture(), reflections[1]->GetCubeMapTexture(), blend);
	}
	else if(reflections.size() == 1)
		shader->SetReflectionTextures(reflections[0]->GetCubeMapTexture(), 0, 0.0);
	else if(world->GetSkyBox())
		shader->SetReflectionTextures(world->GetSkyBox()->GetCubeMap(), 0, 0.0);
	else
		shader->SetReflectionTextures(0, 0, 0.0);
}




void tDeferredRenderer::LegacyLightPass(void)
{
	tAmbientLightingShader *ambient_shader;

	if(ssao && ssao_ambient_only)
	{
		ambient_shader = ssao_ambient_lighting_shader;
		ambient_shader->Bind();
		ambient_shader->SetSSAOTexture(ssao->GetSSAOTexture());
	}
	else
	{
		ambient_shader = ambient_lighting_shader;
		ambient_shader->Bind();
	}

	ambient_shader->SetAmbientLight(world->GetAmbientColor());
	ambient_shader->SetCameraPosition(current_rendering_camera->GetPosition());

	SetReflections(ambient_shader, current_rendering_camera->GetPosition()); // TODO: For VR, a common position for both eyes has to be used

	RenderScreenQuad();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	// point lights
	vector<tPointLight *> point_lights;

	set<tPointLight *>::iterator point_light_sit;
	for(point_light_sit=current_rendering_render_space->point_lights.begin(); point_light_sit!=current_rendering_render_space->point_lights.end(); point_light_sit++)
	{
		tPointLight *light = *point_light_sit;
		if(!light->GetEnabled())
			continue;
		point_lights.push_back(light);
	}

	int point_lights_count = point_lights.size();
	float *point_lights_pos = new float[point_lights_count*3];
	float *point_lights_color = new float[point_lights_count*3];
	float *point_lights_dist = new float[point_lights_count];
	int *point_lights_shadow_enabled = new int[point_lights_count];
	GLuint *point_lights_shadow_maps = new GLuint[point_lights_count];


	vector<tPointLight *>::iterator point_light_it;
	int point_light_i = 0;
	for(point_light_it=point_lights.begin(); point_light_it!=point_lights.end(); point_light_it++, point_light_i++)
	{
		tPointLight *light = *point_light_it;
		memcpy(point_lights_pos + 3*point_light_i, light->GetPosition().v, sizeof(float)*3);
		memcpy(point_lights_color + 3*point_light_i, light->GetColor().v, sizeof(float)*3);
		point_lights_dist[point_light_i] = light->GetDistance();

		if(light->GetShadowEnabled())
		{
			point_lights_shadow_enabled[point_light_i] = 1;
			point_lights_shadow_maps[point_light_i] = light->GetShadow()->GetShadowMap();
		}
		else
		{
			point_lights_shadow_enabled[point_light_i] = 0;
			point_lights_shadow_maps[point_light_i] = 0;
		}
	}

	int point_lights_rendered = 0;
	int shader_passes;
	for(vector<tPointLightingShader *>::iterator shader_i=point_lighting_shaders.begin(); shader_i!=point_lighting_shaders.end(); shader_i++)
	{
		tPointLightingShader *shader = *shader_i;

		if(shader->GetLightsCount() > point_lights_count - point_lights_rendered)
			continue;

		shader_passes = (point_lights_count - point_lights_rendered) / shader->GetLightsCount();

		shader->Bind();
		shader->SetCameraPosition(current_rendering_camera->GetPosition());

		for(int pass=0; pass<shader_passes; pass++)
		{
			shader->SetPointLights(	point_lights_pos + 3*point_lights_rendered,
									point_lights_color + 3*point_lights_rendered,
									point_lights_dist + point_lights_rendered,
									point_lights_shadow_enabled + point_lights_rendered,
									point_lights_shadow_maps + point_lights_rendered);
			RenderScreenQuad();

			point_lights_rendered += shader->GetLightsCount();
		}
	}

	delete[] point_lights_pos;
	delete[] point_lights_color;
	delete[] point_lights_dist;
	delete[] point_lights_shadow_enabled;
	delete[] point_lights_shadow_maps;
}



void tDeferredRenderer::ForwardPass(void)
{
	shadow_pass = false;

	current_rendering_render_space->ForwardPass(this);

	bool particle_render_initialized = false;
	for(int i=0; i<world->GetParticleSystemsCount(); i++)
	{
		tParticleSystem *ps = world->GetParticleSystem(i);

		if(ps->GetRenderType() != tParticleSystem::FORWARD_ADD && ps->GetRenderType() != tParticleSystem::FORWARD_ALPHA)
			continue;

		if(!particle_render_initialized)
		{
			glEnable(GL_DEPTH_TEST);

			particle_forward_shader->Bind();
			particle_forward_shader->SetModelViewProjectionMatrices(current_rendering_camera->GetModelViewMatrix().GetData(), current_rendering_camera->GetProjectionMatrix().GetData());

			particle_render_initialized = true;
		}

		ps->Render(this, particle_forward_shader);
	}

	glDepthMask(GL_TRUE);
}

void tDeferredRenderer::RefractionPass(void)
{
	shadow_pass = false;
	current_rendering_render_space->RefractionPass(this);
}



void tDeferredRenderer::ChangeScreenSize(int width, int height)
{
	if(screen_width == width && screen_height == height)
		return;

	screen_width = width;
	screen_height = height;

	glBindTexture(GL_TEXTURE_2D, color_tex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, color_tex[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	gbuffer->ChangeSize(width, height);

	if(ssao)
		ssao->ChangeScreenSize(width, height);

	glBindTexture(GL_TEXTURE_2D, 0);
}






