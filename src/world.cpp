
#include "towerengine.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"

tWorld::tWorld(void)
{
	ambient_color = Vec(0.1, 0.1, 0.1);
	sky_box = 0;

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
	o->AddedObjectToWorld(this);
}

void tWorld::RemoveObject(tObject *o)
{
	vector<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(*i == o)
		{
			objects.erase(i);
			o->RemovedObjectFromWorld(this);
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


void tWorld::FillRenderObjectSpace(tRenderObjectSpace *space, tCulling *culling, bool clear, bool init_culling)
{
	if(clear)
		space->Clear();

	if(init_culling)
		culling->InitCulling();

	vector<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
	{
		if(culling->TestBoundingBoxCulling((*i)->GetBoundingBox()))
			continue;
		space->objects.insert((*i));
	}
}


void tWorld::FillRenderSpace(tRenderSpace *space, tCulling *culling, bool init_culling)
{
	tBoundingBox b;
	tVector minv, maxv;
	tVector light_pos;
	float light_dist;
	vector<tObject *>::iterator i;

	if(init_culling)
		culling->InitCulling();

	space->Clear();

	FillRenderObjectSpace(space, culling, false, false);

	vector<tPointLight *>::iterator pi;

	for(pi=point_lights.begin(); pi!=point_lights.end(); pi++)
	{
		if(!(*pi)->GetEnabled())
			continue;

		light_pos = (*pi)->GetPosition();
		light_dist = (*pi)->GetDistance();

		if(culling->TestSphereCulling(light_pos, light_dist))
			continue;

		space->point_lights.insert(*pi);
	}

	vector<tDirectionalLight *>::iterator di;

	for(di=dir_lights.begin(); di!=dir_lights.end(); di++)
	{
		space->dir_lights.insert(*di);

		if(!(*di)->GetShadowEnabled())
			continue;

		for(i=objects.begin(); i!=objects.end(); i++)
			(*di)->GetShadow()->GetRenderSpace()->objects.insert((*i)); // TODO: move to tRenderer
	}
}


void tWorld::Step(float time)
{
	physics.dynamics_world->stepSimulation(time, 10);
}




