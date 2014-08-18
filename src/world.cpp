
#include "towerengine.h"

CWorld::CWorld(void)
{
	ambient_color = Vec(0.1, 0.1, 0.1);
	world_object = 0;
	sky_box = 0;
	camera = new CCamera();
}

CWorld::~CWorld(void)
{
	if(world_object)
		delete world_object;
}

void CWorld::AddObject(CObject *o)
{
	vector<CObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(*i == o)
			return;

	objects.push_back(o);
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
	if(point_lights.size() >= CFaceShader::max_point_lights)
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
	if(dir_lights.size() >= CFaceShader::max_directional_lights)
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

void CWorld::RenderShadow(void)
{
	PutToGL(Vec(0.0, 0.0, 0.0));
}

void CWorld::SetShadowTextureMatrix(void)
{
	static double modelView[16];
	static double projection[16];

	// This is matrix transform every coordinate x,y,z
	// x = x* 0.5 + 0.5
	// y = y* 0.5 + 0.5
	// z = z* 0.5 + 0.5
	// Moving from unit cube [-1,1] to [0,1]
	const GLdouble bias[16] = {
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0};

	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE5);

	glLoadIdentity();
	glLoadMatrixd(bias);

	// concatating all matrices into one.
	glMultMatrixd (projection);
	glMultMatrixd (modelView);

	// Go back to normal matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void CWorld::RenderShadowMaps(void)
{
	vector<CPointLight *>::iterator pi;

	for(pi=point_lights.begin(); pi!=point_lights.end(); pi++)
		(*pi)->RenderShadow(this);

	vector<CDirectionalLight *>::iterator di;

	for(di=dir_lights.begin(); di!=dir_lights.end(); di++)
		(*di)->RenderShadow(this);
}

void CWorld::Render(int screen_width, int screen_height)
{
	camera->SetAspect((float)screen_width / (float)screen_height);

	RenderShadowMaps();

	int i;
	CPointLight *point_light;
	float *point_light_pos, *point_light_color, *point_light_distance;
	int *point_light_shadow_enabled;
	GLuint *point_light_shadow_maps;

	point_light_pos = new float[point_lights.size() * 3];
	point_light_color = new float[point_lights.size() * 3];
	point_light_distance = new float[point_lights.size()];
	point_light_shadow_enabled = new int[point_lights.size()];
	point_light_shadow_maps = new GLuint[point_lights.size()];

	for(i=0; i<(int)point_lights.size(); i++)
	{
		point_light = point_lights[i];
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

	dir_light_dir = new float[dir_lights.size() * 3];
	dir_light_color = new float[dir_lights.size() * 3];
	dir_light_shadow_enabled = new int[dir_lights.size()];
	dir_light_shadow_clip = new float[dir_lights.size() * 2];
	dir_light_shadow_tex_matrix = new float[dir_lights.size() * 16 * CFaceShader::max_directional_light_splits];
	dir_light_shadow_splits_count = new float[dir_lights.size()];
	dir_light_shadow_splits_z = new float[dir_lights.size() * (CFaceShader::max_directional_light_splits+1)];
	dir_light_shadow_maps = new GLuint[dir_lights.size()];

	for(i=0; i<(int)dir_lights.size(); i++)
	{
		dir_light = dir_lights[i];
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
				memcpy(dir_light_shadow_tex_matrix + i*16*CFaceShader::max_directional_light_splits + 16*s,
						dir_light->GetShadow()->GetTextureMatrix()[s], sizeof(float) * 16);
			}
			memcpy(dir_light_shadow_splits_z + i*(CFaceShader::max_directional_light_splits+1),
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

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera->GetAngle(), camera->GetAspect(), camera->GetNearClip(), camera->GetFarClip());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	CVector cam_to = camera->GetPosition() + camera->GetDirection();
	CVector cam_pos = camera->GetPosition();
	gluLookAt(cam_pos.x, cam_pos.y, cam_pos.z, cam_to.x, cam_to.y, cam_to.z, 0.0, 1.0, 0.0); //pos x, y, z, to x, y, z

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);


	if(sky_box)
		sky_box->Paint(cam_pos);

	CEngine::SetCurrentFaceShader(CEngine::GetDefaultFaceShader());
	CEngine::BindCurrentFaceShader();
	CEngine::GetCurrentFaceShader()->SetPointLights(	point_lights.size(),
														point_light_pos,
														point_light_color,
														point_light_distance,
														point_light_shadow_enabled,
														point_light_shadow_maps);
	CEngine::GetCurrentFaceShader()->SetDirectionalLights(dir_lights.size(), dir_light_dir, dir_light_color, dir_light_shadow_enabled, dir_light_shadow_maps, dir_light_shadow_clip, dir_light_shadow_tex_matrix, dir_light_shadow_splits_count, dir_light_shadow_splits_z);
	CEngine::GetCurrentFaceShader()->SetLightAmbientColor(ambient_color);
	CEngine::GetCurrentFaceShader()->SetTwoSide(0);
	//CEngine::GetFaceShader()->SetClip(clip.v, clip.d);

	PutToGL(cam_pos);

	delete[] point_light_pos;
	delete[] point_light_color;
	delete[] point_light_distance;
}

void CWorld::PutToGL(CVector cam_pos)
{
	vector<CObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++) // TODO: Sort objects of CWorld by z-distance before painting
	{
		(*i)->PutToGL(cam_pos);
	}
}
