
#include "towerengine.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"

tWorld::tWorld(void)
{
	ambient_color = Vec(0.1, 0.1, 0.1);
	sky_box = 0;
	camera = new tCamera();
	camera_render_space = new tRenderSpace();

	physics.broadphase = new btDbvtBroadphase();
	physics.collision_configuration = new btDefaultCollisionConfiguration();
	physics.collision_dispatcher = new btCollisionDispatcher(physics.collision_configuration);
	physics.solver = new btSequentialImpulseConstraintSolver();
	physics.dynamics_world = new btDiscreteDynamicsWorld(physics.collision_dispatcher, physics.broadphase, physics.solver, physics.collision_configuration);
	physics.dynamics_world->getDispatchInfo().m_allowedCcdPenetration=0.0001f;
}

tWorld::~tWorld(void)
{
	delete physics.broadphase;
	delete physics.collision_configuration;
	delete physics.collision_dispatcher;
	delete physics.solver;
	delete physics.dynamics_world;
}

void tWorld::AddObject(tObject *o)
{
	vector<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(*i == o)
			return;

	objects.push_back(o);

	if(o->GetRigidBody())
		physics.dynamics_world->addRigidBody(o->GetRigidBody());
}

void tWorld::RemoveObject(tObject *o)
{
	vector<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(*i == o)
		{
			if((*i)->GetRigidBody())
				physics.dynamics_world->removeRigidBody((*i)->GetRigidBody());
			objects.erase(i);
			return;
		}
}

void tWorld::AddPointLight(tPointLight *light)
{
	if(point_lights.size() >= tLightPassShader::max_point_lights)
		return;

	for(vector<tPointLight *>::iterator i = point_lights.begin(); i != point_lights.end(); i++)
		if((*i) == light)
			return;
	point_lights.push_back(light);
}

void tWorld::RemovePointLight(tPointLight *light)
{
	for(vector<tPointLight *>::iterator i = point_lights.begin(); i != point_lights.end(); i++)
		if((*i) == light)
		{
			point_lights.erase(i);
			return;
		}
}

void tWorld::AddDirectionalLight(tDirectionalLight *light)
{
	if(dir_lights.size() >= tLightPassShader::max_directional_lights)
		return;

	for(vector<tDirectionalLight *>::iterator i = dir_lights.begin(); i != dir_lights.end(); i++)
		if((*i) == light)
			return;
	dir_lights.push_back(light);
}

void tWorld::RemoveDirectionalLight(tDirectionalLight *light)
{
	for(vector<tDirectionalLight *>::iterator i = dir_lights.begin(); i != dir_lights.end(); i++)
		if((*i) == light)
		{
			dir_lights.erase(i);
			return;
		}
}

void tWorld::Clear(void)
{
	objects.clear();
}

void tWorld::RenderShadowMaps(void)
{
	set<tPointLight *>::iterator pi;

	for(pi=camera_render_point_lights.begin(); pi!=camera_render_point_lights.end(); pi++)
		(*pi)->RenderShadow(this);

	set<tDirectionalLight *>::iterator di;

	for(di=camera_render_dir_lights.begin(); di!=camera_render_dir_lights.end(); di++)
		(*di)->RenderShadow(this);
}

void tWorld::FillRenderSpaces(void)
{
	tBoundingBox b;
	tVector minv, maxv;
	tVector light_pos;
	float light_dist;
	vector<tObject *>::iterator i;

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

	vector<tPointLight *>::iterator pi;

	for(pi=point_lights.begin(); pi!=point_lights.end(); pi++)
	{
		light_pos = (*pi)->GetPosition();
		light_dist = (*pi)->GetDistance();

		if(camera->TestSphereCulling(light_pos, light_dist))
			continue;

		camera_render_point_lights.insert(*pi);

		if(!(*pi)->GetShadowEnabled())
			continue;

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

	vector<tDirectionalLight *>::iterator di;

	for(di=dir_lights.begin(); di!=dir_lights.end(); di++)
	{
		camera_render_dir_lights.insert(*di);

		if(!(*di)->GetShadowEnabled())
			continue;

		for(i=objects.begin(); i!=objects.end(); i++)
			(*di)->GetShadow()->GetRenderSpace()->objects.insert((*i));
	}
}


void tWorld::Step(float time)
{
	physics.dynamics_world->stepSimulation(time, 10);
}


void tWorld::GetPointLightUniforms(int &count, float *pos, float *color, float *distance, int *shadow_enabled, GLuint *shadow_maps)
{
	int i;
	tPointLight *point_light;
	set<tPointLight *>::iterator pi;

	count = camera_render_point_lights.size();

	for(pi=camera_render_point_lights.begin(), i=0; pi!=camera_render_point_lights.end() && i<tLightPassShader::max_point_lights; pi++, i++)
	{
		point_light = *pi;
		memcpy(pos + i*3, point_light->GetPosition().v, 3 * sizeof(float));
		memcpy(color + i*3, point_light->GetColor().v, 3 * sizeof(float));
		distance[i] = point_light->GetDistance();
		shadow_enabled[i] = (point_light->GetShadowEnabled() ? 1 : 0);
		shadow_maps[i] = point_light->GetShadowEnabled() ? point_light->GetShadow()->GetShadowMap() : 0;
	}

}

void tWorld::GetDirectionalLightUniforms(int &count, float *dir, float *color, float *shadow_clip, float *shadow_tex_matrix, float *shadow_splits_count, float *shadow_splits_z, int *shadow_enabled, GLuint *shadow_maps)
{
	int i, s;
	tDirectionalLight *dir_light;
	set<tDirectionalLight *>::iterator di;

	count = camera_render_dir_lights.size();

	for(di=camera_render_dir_lights.begin(), i=0; di!=camera_render_dir_lights.end() && i<tLightPassShader::max_directional_lights; di++, i++)
	{
		dir_light = *di;
		memcpy(dir + i*3, dir_light->GetDirection().v, 3 * sizeof(float));
		memcpy(color + i*3, dir_light->GetColor().v, 3 * sizeof(float));
		if(dir_light->GetShadowEnabled())
		{
			shadow_enabled[i] = 1;
			shadow_clip[i*2 + 0] = dir_light->GetShadow()->GetNearClip();
			shadow_clip[i*2 + 1] = dir_light->GetShadow()->GetFarClip();
			shadow_maps[i] = dir_light->GetShadow()->GetShadowMap();
			shadow_splits_count[i] = dir_light->GetShadow()->GetSplitsCount();
			for(s=0; s<dir_light->GetShadow()->GetSplitsCount(); s++)
			{
				memcpy(shadow_tex_matrix + i*16*tLightPassShader::max_directional_light_splits + 16*s,
						dir_light->GetShadow()->GetTextureMatrix()[s], sizeof(float) * 16);
			}
			memcpy(shadow_splits_z + i*(tLightPassShader::max_directional_light_splits+1),
					dir_light->GetShadow()->GetSplitsZ(), sizeof(float) * (dir_light->GetShadow()->GetSplitsCount()+1));
		}
		else
		{
			shadow_enabled[i] = 0;
			shadow_clip[i*2 + 0] = 0.0;
			shadow_clip[i*2 + 1] = 0.0;
			memcpy(shadow_tex_matrix + i*16, tEngine::identity_matrix4, sizeof(float) * 16);
			shadow_maps[i] = 0;
		}
	}
}





