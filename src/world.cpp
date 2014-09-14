
#include "towerengine.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"

CWorld::CWorld(int screen_width, int screen_height)
{
	this->screen_width = screen_width;
	this->screen_height = screen_height;
	ambient_color = Vec(0.1, 0.1, 0.1);
	world_object = 0;
	sky_box = 0;
	camera = new CCamera();
	camera_render_space = new CRenderSpace();

	gbuffer = new CGBuffer(screen_width, screen_height);

	physics.broadphase = new btDbvtBroadphase();
	physics.collision_configuration = new btDefaultCollisionConfiguration();
	physics.collision_dispatcher = new btCollisionDispatcher(physics.collision_configuration);
	physics.solver = new btSequentialImpulseConstraintSolver();
	physics.dynamics_world = new btDiscreteDynamicsWorld(physics.collision_dispatcher, physics.broadphase, physics.solver, physics.collision_configuration);
	physics.dynamics_world->getDispatchInfo().m_allowedCcdPenetration=0.0001f;
}

CWorld::~CWorld(void)
{
	if(world_object)
		delete world_object;

	delete physics.broadphase;
	delete physics.collision_configuration;
	delete physics.collision_dispatcher;
	delete physics.solver;
	delete physics.dynamics_world;
}

void CWorld::AddObject(CObject *o)
{
	vector<CObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(*i == o)
			return;

	objects.push_back(o);

	physics.dynamics_world->addRigidBody(o->GetRigidBody());
}

void CWorld::RemoveObject(CObject *o)
{
	vector<CObject *>::iterator i;

	if(o == world_object)
	{
		world_object = 0;
		ambient_color = Vec(0.1, 0.1, 0.1);
	}

	for(i=objects.begin(); i!=objects.end(); i++)
		if(*i == o)
		{
			objects.erase(i);
			return;
		}
}

void CWorld::AddPointLight(CPointLight *light)
{
	if(point_lights.size() >= CLightPassShader::max_point_lights)
		return;

	for(vector<CPointLight *>::iterator i = point_lights.begin(); i != point_lights.end(); i++)
		if((*i) == light)
			return;
	point_lights.push_back(light);
}

void CWorld::RemovePointLight(CPointLight *light)
{
	for(vector<CPointLight *>::iterator i = point_lights.begin(); i != point_lights.end(); i++)
		if((*i) == light)
		{
			point_lights.erase(i);
			return;
		}
}

void CWorld::AddDirectionalLight(CDirectionalLight *light)
{
	if(dir_lights.size() >= CLightPassShader::max_directional_lights)
		return;

	for(vector<CDirectionalLight *>::iterator i = dir_lights.begin(); i != dir_lights.end(); i++)
		if((*i) == light)
			return;
	dir_lights.push_back(light);
}

void CWorld::RemoveDirectionalLight(CDirectionalLight *light)
{
	for(vector<CDirectionalLight *>::iterator i = dir_lights.begin(); i != dir_lights.end(); i++)
		if((*i) == light)
		{
			dir_lights.erase(i);
			return;
		}
}


void CWorld::SetWorldMesh(CMesh *m, bool load_light)
{
	map<string, CEntity *> light_entities;
	map<string, CEntity *>::iterator ei;
	map<string, CEntityAttribute *>::iterator ai;
	CEntity *light_entity;
	CVector light_pos, light_color;
	CPointLight *point_light;

	RemoveWorldMesh();
	AddObject(world_object = new CMeshObject(m));

	if(load_light)
	{
		light_pos = Vec(0.0, 0.0, 0.0);
		light_color = Vec(1.0, 1.0, 1.0);
		ambient_color = Vec(0.1, 0.1, 0.1);

		light_entities = m->GetEntitiesInGroup("lights");
		if((ei = light_entities.find(string("light"))) != light_entities.end())
		{
			light_entity = ei->second;

			if((ai = light_entity->attributes.find(string("pos"))) != light_entity->attributes.end())
				if(ai->second->type == CEntityAttribute::VECTOR)
					light_pos = ai->second->vec_v;

			if((ai = light_entity->attributes.find(string("color"))) != light_entity->attributes.end())
				if(ai->second->type == CEntityAttribute::VECTOR)
					light_color = ai->second->vec_v;

			point_light = new CPointLight(light_pos, light_color, 300.0);
			AddPointLight(point_light);
			point_light->InitShadow(512, true, 0.00001);


			if((ai = light_entity->attributes.find(string("ambient"))) != light_entity->attributes.end())
				if(ai->second->type == CEntityAttribute::VECTOR)
					ambient_color = ai->second->vec_v;
		}
	}
}

void CWorld::RemoveWorldMesh(void)
{
	if(!world_object)
		return;

	CMeshObject *m = world_object;
	RemoveObject(world_object);

	delete m;
}

void CWorld::Clear(void)
{
	RemoveWorldMesh();
	objects.clear();
}

void CWorld::RenderShadowMaps(void)
{
	set<CPointLight *>::iterator pi;

	for(pi=camera_render_point_lights.begin(); pi!=camera_render_point_lights.end(); pi++)
		(*pi)->RenderShadow(this);

	set<CDirectionalLight *>::iterator di;

	for(di=camera_render_dir_lights.begin(); di!=camera_render_dir_lights.end(); di++)
		(*di)->RenderShadow(this);
}

void CWorld::FillRenderSpaces(void)
{
	CBoundingBox b;
	CVector minv, maxv;
	CVector light_pos;
	float light_dist;
	vector<CObject *>::iterator i;

	camera->CalculateFrustumPlanes();

	camera_render_space->ClearObjects();
	camera_render_point_lights.clear();
	camera_render_dir_lights.clear();

	for(i=objects.begin(); i!=objects.end(); i++)
	{
		if(camera->TestBoundingBoxCulling((*i)->GetBoundingBox()))
			continue;
		camera_render_space->objects.insert((*i));
	}

	vector<CPointLight *>::iterator pi;

	for(pi=point_lights.begin(); pi!=point_lights.end(); pi++)
	{
		if(!(*pi)->GetShadowEnabled())
			continue;

		light_pos = (*pi)->GetPosition();
		light_dist = (*pi)->GetDistance();

		if(camera->TestSphereCulling(light_pos, light_dist))
			continue;

		camera_render_point_lights.insert(*pi);

		for(i=objects.begin(); i!=objects.end(); i++)
		{
			b = (*i)->GetBoundingBox();
			minv = b.GetMin(); maxv = b.GetMax();

			if(minv.x > light_pos.x + light_dist)
				continue;
			if(minv.y > light_pos.y + light_dist)
				continue;
			if(minv.z > light_pos.z + light_dist)
				continue;
			if(maxv.x < light_pos.x - light_dist)
				continue;
			if(maxv.y < light_pos.y - light_dist)
				continue;
			if(maxv.z < light_pos.z - light_dist)
				continue;

			(*pi)->GetShadow()->GetRenderSpace()->objects.insert((*i));
		}
	}

	vector<CDirectionalLight *>::iterator di;

	for(di=dir_lights.begin(); di!=dir_lights.end(); di++)
	{
		if(!(*di)->GetShadowEnabled())
			continue;

		camera_render_dir_lights.insert(*di);

		for(i=objects.begin(); i!=objects.end(); i++)
			(*di)->GetShadow()->GetRenderSpace()->objects.insert((*i));
	}
}


void CWorld::Step(float time)
{
	physics.dynamics_world->stepSimulation(time, 10);
}


void CWorld::Render(void)
{
	camera->SetAspect((float)screen_width / (float)screen_height);

	FillRenderSpaces();

	RenderShadowMaps();

	int i;
	CPointLight *point_light;
	float *point_light_pos, *point_light_color, *point_light_distance;
	int *point_light_shadow_enabled;
	GLuint *point_light_shadow_maps;
	set<CPointLight *>::iterator pi;

	point_light_pos = new float[camera_render_point_lights.size() * 3];
	point_light_color = new float[camera_render_point_lights.size() * 3];
	point_light_distance = new float[camera_render_point_lights.size()];
	point_light_shadow_enabled = new int[camera_render_point_lights.size()];
	point_light_shadow_maps = new GLuint[camera_render_point_lights.size()];

	for(pi=camera_render_point_lights.begin(), i=0; pi!=camera_render_point_lights.end(); pi++, i++)
	{
		point_light = *pi;
		memcpy(point_light_pos + i*3, point_light->GetPosition().v, 3 * sizeof(float));
		memcpy(point_light_color + i*3, point_light->GetColor().v, 3 * sizeof(float));
		point_light_distance[i] = point_light->GetDistance();
		point_light_shadow_enabled[i] = (point_light->GetShadowEnabled() ? 1 : 0);
		point_light_shadow_maps[i] = point_light->GetShadowEnabled() ? point_light->GetShadow()->GetShadowMap() : 0;
	}

	int s;
	CDirectionalLight *dir_light;
	float *dir_light_dir, *dir_light_color, *dir_light_shadow_clip, *dir_light_shadow_tex_matrix;
	float *dir_light_shadow_splits_count, *dir_light_shadow_splits_z;
	int *dir_light_shadow_enabled;
	GLuint *dir_light_shadow_maps;
	set<CDirectionalLight *>::iterator di;

	dir_light_dir = new float[camera_render_dir_lights.size() * 3];
	dir_light_color = new float[camera_render_dir_lights.size() * 3];
	dir_light_shadow_enabled = new int[camera_render_dir_lights.size()];
	dir_light_shadow_clip = new float[camera_render_dir_lights.size() * 2];
	dir_light_shadow_tex_matrix = new float[camera_render_dir_lights.size() * 16 * CLightPassShader::max_directional_light_splits];
	dir_light_shadow_splits_count = new float[camera_render_dir_lights.size()];
	dir_light_shadow_splits_z = new float[camera_render_dir_lights.size() * (CLightPassShader::max_directional_light_splits+1)];
	dir_light_shadow_maps = new GLuint[camera_render_dir_lights.size()];

	for(di=camera_render_dir_lights.begin(), i=0; di!=camera_render_dir_lights.end(); di++, i++)
	{
		dir_light = *di;
		memcpy(dir_light_dir + i*3, dir_light->GetDirection().v, 3 * sizeof(float));
		memcpy(dir_light_color + i*3, dir_light->GetColor().v, 3 * sizeof(float));
		if(dir_light->GetShadowEnabled())
		{
			dir_light_shadow_enabled[i] = 1;
			dir_light_shadow_clip[i*2 + 0] = dir_light->GetShadow()->GetNearClip();
			dir_light_shadow_clip[i*2 + 1] = dir_light->GetShadow()->GetFarClip();
			dir_light_shadow_maps[i] = dir_light->GetShadow()->GetShadowMap();
			dir_light_shadow_splits_count[i] = dir_light->GetShadow()->GetSplitsCount();
			for(s=0; s<dir_light->GetShadow()->GetSplitsCount(); s++)
			{
				memcpy(dir_light_shadow_tex_matrix + i*16*CLightPassShader::max_directional_light_splits + 16*s,
						dir_light->GetShadow()->GetTextureMatrix()[s], sizeof(float) * 16);
			}
			memcpy(dir_light_shadow_splits_z + i*(CLightPassShader::max_directional_light_splits+1),
					dir_light->GetShadow()->GetSplitsZ(), sizeof(float) * (dir_light->GetShadow()->GetSplitsCount()+1));
		}
		else
		{
			dir_light_shadow_enabled[i] = 0;
			dir_light_shadow_clip[i*2 + 0] = 0.0;
			dir_light_shadow_clip[i*2 + 1] = 0.0;
			memcpy(dir_light_shadow_tex_matrix + i*16, CEngine::identity_matrix4, sizeof(float) * 16);
			dir_light_shadow_maps[i] = 0;
		}
	}

	GeometryPass();


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
	CEngine::GetLightPassShader()->SetPointLights(	camera_render_point_lights.size(),
														point_light_pos,
														point_light_color,
														point_light_distance,
														point_light_shadow_enabled,
														point_light_shadow_maps);
	CEngine::GetLightPassShader()->SetDirectionalLights(	camera_render_dir_lights.size(),
															dir_light_dir,
															dir_light_color,
															dir_light_shadow_enabled,
															dir_light_shadow_maps,
															dir_light_shadow_clip,
															dir_light_shadow_tex_matrix,
															dir_light_shadow_splits_count,
															dir_light_shadow_splits_z);
	CEngine::GetLightPassShader()->SetLightAmbientColor(ambient_color);
	CEngine::GetLightPassShader()->SetCameraPosition(camera->GetPosition());


	glBegin(GL_QUADS);
	glVertex2f(0.0, 1.0);
	glVertex2f(0.0, 0.0);
	glVertex2f(1.0, 0.0);
	glVertex2f(1.0, 1.0);
	glEnd();

	CShader::Unbind();

	delete[] point_light_pos;
	delete[] point_light_color;
	delete[] point_light_distance;
}


void CWorld::GeometryPass(void)
{
	gbuffer->BindForDrawing();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	camera->SetModelviewProjectionMatrix();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	CEngine::SetCurrentFaceShader(CEngine::GetGeometryPassShader());
	CEngine::BindCurrentFaceShader();

	camera_render_space->Render();

	CShader::Unbind();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

/*void CWorld::PaintObjects(void) // TODO: Sort objects of CWorld by z-distance before painting
{
	vector<CObject *>::iterator i;
	CBoundingBox b;
	CVector minv, maxv;
	CVector light_pos;
	float light_dist;

	switch(camera_mode)
	{
		case POINT_LIGHT:
			light_pos = point_light_camera->GetPosition();
			light_dist = point_light_camera->GetDistance();

			for(i=objects.begin(); i!=objects.end(); i++)
			{
				b = (*i)->GetBoundingBox();
				minv = b.GetMin(); maxv = b.GetMax();

				if(minv.x > light_pos.x + light_dist)
					continue;
				if(minv.y > light_pos.y + light_dist)
					continue;
				if(minv.z > light_pos.z + light_dist)
					continue;
				if(maxv.x < light_pos.x - light_dist)
					continue;
				if(maxv.y < light_pos.y - light_dist)
					continue;
				if(maxv.z < light_pos.z - light_dist)
					continue;

				(*i)->PutToGL(cam_pos);
			}
			break;

		default:
			for(i=objects.begin(); i!=objects.end(); i++)
				(*i)->PutToGL(cam_pos);
			break;
	}
}*/





