
#include "towerengine.h"

using namespace std;

tCubeMapReflection::tCubeMapReflection(tRenderer *renderer, int resolution, tVector position)
{
	this->renderer = renderer;
	this->resolution = resolution;
	this->position = position;

	glGenFramebuffers(1, &fbo);

	gbuffer = new tGBuffer(resolution, resolution, fbo, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &color_tex);

	glBindTexture(GL_TEXTURE_CUBE_MAP, color_tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for(int i=0; i<6; i++)
		glTexImage2D(CubeTex(i), 0, GL_RGBA, resolution, resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenRenderbuffers(1, &depth_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	camera = new tCamera();
	camera->SetAngle(90.0);
	camera->SetAspect(1.0);
	// TODO: near/far clip other than default

	render_space = new tRenderSpace();

	invalid = true;
}

tCubeMapReflection::~tCubeMapReflection(void)
{
	delete camera;
	delete render_space;

	glDeleteFramebuffers(1, &fbo);

	glDeleteTextures(1, &color_tex);
	glDeleteRenderbuffers(1, &depth_rbo);

	delete gbuffer;
}


void tCubeMapReflection::Render(void)
{
	tWorld *world = renderer->GetWorld();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glViewport(0, 0, resolution, resolution);

	for(int s=0; s<6; s++)
	{
		GeometryPass(s, world);
		LightPass(s, world);
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);


	glBindTexture(GL_TEXTURE_CUBE_MAP, color_tex);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	invalid = false;
}

void tCubeMapReflection::GeometryPass(int side, tWorld *world)
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

	world->FillRenderSpace(render_space, camera);

	gbuffer->BindDrawBuffers();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	tGeometryPassShader *shader = renderer->GetGeometryPassShader();

	renderer->SetCurrentFaceShader(renderer->GetGeometryPassShader());
	renderer->BindCurrentFaceShader();

	shader->SetModelViewProjectionMatrix(camera->GetModelViewProjectionMatrix().GetData());
	shader->SetCameraPosition(camera->GetPosition());

	render_space->GeometryPass(renderer);
}

void tCubeMapReflection::LightPass(int side, tWorld *world)
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
		skybox_shader->SetModelViewProjectionMatrix(camera->GetModelViewProjectionMatrix().GetData());
		skybox_shader->SetCameraPosition(camera->GetPosition());
		sky_box->Paint(renderer, camera->GetPosition());
	}

	gbuffer->BindTextures();

	renderer->GetAmbientLightingShader()->Bind();
	renderer->GetAmbientLightingShader()->SetAmbientLight(world->GetAmbientColor());

	renderer->RenderScreenQuad();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	renderer->GetDirectionalLightingShader()->Bind();
	renderer->GetDirectionalLightingShader()->SetCameraPosition(camera->GetPosition());

	set<tDirectionalLight *>::iterator dir_light_it;
	for(dir_light_it=render_space->dir_lights.begin(); dir_light_it!=render_space->dir_lights.end(); dir_light_it++)
	{
		(*dir_light_it)->InitRenderLighting(renderer->GetDirectionalLightingShader());
		renderer->RenderScreenQuad();
	}


	renderer->GetPointLightingShader()->Bind();
	renderer->GetPointLightingShader()->SetCameraPosition(camera->GetPosition());

	set<tPointLight *>::iterator point_light_it;
	for(point_light_it=render_space->point_lights.begin(); point_light_it!=render_space->point_lights.end(); point_light_it++)
	{
		tPointLight *light = *point_light_it;
		if(!light->GetEnabled())
			continue;
		light->InitRenderLighting(renderer->GetPointLightingShader());
		renderer->RenderScreenQuad();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
