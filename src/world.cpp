
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





