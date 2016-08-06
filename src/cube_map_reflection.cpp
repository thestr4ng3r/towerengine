
#include "towerengine.h"

using namespace std;

tCubeMapReflection::tCubeMapReflection(tVector position)
{
	this->initialized = false;
	this->resolution = 0;
	this->position = position;

	camera = new tCamera();
	camera->SetFOV(-1.0f, 1.0f, 1.0f, -1.0f);
	// TODO: near/far clip other than default

	render_space = new tRenderSpace();

	fbo = 0;
	color_tex = 0;
}

tCubeMapReflection::~tCubeMapReflection(void)
{
	delete camera;
	delete render_space;

	glDeleteFramebuffers(1, &fbo);

	glDeleteTextures(1, &color_tex);

	delete gbuffer;
}

void tCubeMapReflection::Init(unsigned int resolution_log)
{
	this->resolution_log = resolution_log;
	this->resolution = (unsigned int)1 << resolution_log;

	glGenFramebuffers(1, &fbo);

	gbuffer = new tGBuffer(resolution, resolution, fbo, 1);



	glGenTextures(1, &color_tex);

	glBindTexture(GL_TEXTURE_CUBE_MAP, color_tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for(int i=0; i<6; i++)
		glTexImage2D(CubeTex(i), 0, GL_RGBA, resolution, resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);


	glGenTextures(1, &blur_tex);

	glBindTexture(GL_TEXTURE_CUBE_MAP, blur_tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for(int i=0; i<6; i++)
		glTexImage2D(CubeTex(i), 0, GL_RGBA, resolution, resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);


	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenFramebuffers(1, &blur_fbo);

	invalid = true;
	initialized = true;
}

void tCubeMapReflection::Render(tRenderer *renderer)
{
	if(!initialized)
		return;

	tWorld *world = renderer->GetWorld();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glViewport(0, 0, resolution, resolution);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	for(int s=0; s<6; s++)
	{
		GeometryPass(renderer, s, world);
		LightPass(renderer, s, world);
	}

	glDisable(GL_BLEND);


	glBindTexture(GL_TEXTURE_CUBE_MAP, color_tex);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	BlurPass(renderer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	invalid = false;
}

void tCubeMapReflection::GeometryPass(tRenderer *renderer, int side, tWorld *world)
{
	camera->SetPosition(position);
	camera->SetDirection(CubeVecS(side));

	tVector cam_up = Vec(0.0, -1.0, 0.0);
	if(side == 2)
		cam_up = Vec(0.0, 0.0, 1.0);
	else if(side == 3)
		cam_up = Vec(0.0, 0.0, -1.0);

	camera->SetUp(cam_up);
	camera->CalculateModelViewProjectionMatrix();
	renderer->GetMatrixBuffer()->Bind();
	renderer->GetMatrixBuffer()->UpdateBuffer(camera->GetModelViewProjectionMatrix());
	renderer->GetPositionRestoreDataBuffer()->Bind();
	renderer->GetPositionRestoreDataBuffer()->UpdateBuffer(camera);

	world->FillRenderSpace(render_space, (tCulling **)&camera, 1);

	gbuffer->BindDrawBuffers();

	glClear(GL_DEPTH_BUFFER_BIT);

	float clear_color[] = {0.0, 0.0, 0.0, 0.0};
	glClearBufferfv(GL_COLOR, gbuffer->GetDrawBufferIndex(tGBuffer::BASE_COLOR_TEX), clear_color);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	tGeometryPassShader *shader = renderer->GetGeometryPassShader();

	renderer->SetCurrentFaceShader(renderer->GetGeometryPassShader());
	renderer->BindCurrentFaceShader();

	shader->SetCubeMapReflectionTexture(0);

	shader->SetCameraPosition(camera->GetPosition());

	render_space->GeometryPass(renderer, false);
}

void tCubeMapReflection::LightPass(tRenderer *renderer, int side, tWorld *world)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+side, color_tex, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	tSkyBox *sky_box = world->GetSkyBox();

	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	if(sky_box)
	{
		tSkyBoxShader *skybox_shader = renderer->GetSkyBoxShader();
		skybox_shader->Bind();
		skybox_shader->SetCameraPosition(camera->GetPosition());
		sky_box->Paint(renderer, camera->GetPosition());
	}

	gbuffer->BindTextures();

	renderer->GetAmbientLightingShader()->Bind();
	renderer->GetAmbientLightingShader()->SetAmbientLight(world->GetAmbientColor());

	renderer->RenderScreenQuad();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);


	// directional lights
	renderer->GetDirectionalLightingShader()->Bind();
	renderer->GetDirectionalLightingShader()->SetCameraPosition(camera->GetPosition());

	set<tDirectionalLight *>::iterator dir_light_it;
	for(dir_light_it=render_space->dir_lights.begin(); dir_light_it!=render_space->dir_lights.end(); dir_light_it++)
	{
		(*dir_light_it)->InitRenderLighting(renderer->GetDirectionalLightingShader());
		renderer->RenderScreenQuad();
	}





	// point lights
	vector<tPointLight *> point_lights;

	set<tPointLight *>::iterator point_light_sit;
	for(point_light_sit=render_space->point_lights.begin(); point_light_sit!=render_space->point_lights.end(); point_light_sit++)
	{
		tPointLight *light = *point_light_sit;
		if(!light->GetEnabled())
			continue;
		point_lights.push_back(light);
	}

	int point_lights_count = point_lights.size();
	float *point_lights_pos = new float[point_lights_count * 3];
	float *point_lights_color = new float[point_lights_count * 3];
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
	for(vector<tPointLightingShader *>::iterator shader_i=renderer->GetPointLightingShadersBegin(); shader_i!=renderer->GetPointLightingShadersEnd(); shader_i++)
	{
		tPointLightingShader *shader = *shader_i;

		if(shader->GetLightsCount() > point_lights_count - point_lights_rendered)
			continue;

		shader_passes = (point_lights_count - point_lights_rendered) / shader->GetLightsCount();

		shader->Bind();
		shader->SetCameraPosition(camera->GetPosition());

		for(int pass=0; pass<shader_passes; pass++)
		{
			shader->SetPointLights(	point_lights_pos + 3*point_lights_rendered,
									point_lights_color + 3*point_lights_rendered,
									point_lights_dist + point_lights_rendered,
									point_lights_shadow_enabled + point_lights_rendered,
									point_lights_shadow_maps + point_lights_rendered);
			renderer->RenderScreenQuad();

			point_lights_rendered += shader->GetLightsCount();
		}
	}

	delete[] point_lights_pos;
	delete[] point_lights_color;
	delete[] point_lights_dist;
	delete[] point_lights_shadow_enabled;
	delete[] point_lights_shadow_maps;


	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static const GLenum blur_draw_buffers[6] = {	GL_COLOR_ATTACHMENT0,
												GL_COLOR_ATTACHMENT1,
												GL_COLOR_ATTACHMENT2,
												GL_COLOR_ATTACHMENT3,
												GL_COLOR_ATTACHMENT4,
												GL_COLOR_ATTACHMENT5 };


void tCubeMapReflection::BlurPass(tRenderer *renderer)
{
	tCubeMapBlurShader *shader = renderer->GetCubeMapBlurShader();

	glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo);

	shader->Bind();
	shader->SetTexture(color_tex);

	glDisable(GL_DEPTH_TEST);

	glDrawBuffers(6, blur_draw_buffers);

	for(unsigned int direction=0; direction <= 1; direction++)
	{
		shader->SetTexture(direction ? color_tex : blur_tex);

		for(unsigned int level = 0; level <= resolution_log - 2; level++)
		{
			for(unsigned int side = 0; side < 6; side++)
				glFramebufferTexture2D(GL_FRAMEBUFFER,
									   blur_draw_buffers[side],
									   GL_TEXTURE_CUBE_MAP_POSITIVE_X + side,
									   direction ? color_tex : blur_tex,
									   level);

			tVector blur_dir;

			int level_resolution = 1 << (resolution_log - level);
			glViewport(0, 0, level_resolution, level_resolution);

			if(direction == 0)
				blur_dir = Vec(1.0, 0.0, 0.0) * (1.0f / (float)level_resolution) * 2.0f;
			else // direction == 1
				blur_dir = Vec(0.0, 1.0, 0.0) * (1.0f / (float)level_resolution) * 2.0f;

			shader->SetBlurDir(blur_dir);
			shader->SetMipmapLevel(level);

			renderer->RenderScreenQuad();
		}
	}
}