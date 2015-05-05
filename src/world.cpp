
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

void tWorld::RenderShadowMaps(tRenderer *renderer)  // TODO: move to tRenderer
{
	list<tPointLight *>::iterator pi;

	for(pi=render_point_light_shadows.begin(); pi!=render_point_light_shadows.end(); pi++)
		(*pi)->RenderShadow(renderer);

	set<tDirectionalLight *>::iterator di;

	for(di=camera_render_dir_lights.begin(); di!=camera_render_dir_lights.end(); di++)
		(*di)->RenderShadow(renderer);
}

bool CompareFloatComparable(tComparable<float> *a, tComparable<float> *b)
{
	return a->GetCompareValue() < b->GetCompareValue();
}


void tWorld::FillRenderSpaces(int point_light_shadow_limit)
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
	render_point_light_shadows.clear();

	for(i=objects.begin(); i!=objects.end(); i++)
	{
		if(camera->TestBoundingBoxCulling((*i)->GetBoundingBox()))
			continue;
		camera_render_space->objects.insert((*i));
	}

	vector<tPointLight *>::iterator pi;

	for(pi=point_lights.begin(); pi!=point_lights.end(); pi++)
	{
		if(!(*pi)->GetEnabled())
			continue;

		light_pos = (*pi)->GetPosition();
		light_dist = (*pi)->GetDistance();

		if(camera->TestSphereCulling(light_pos, light_dist))
			continue;

		camera_render_point_lights.insert(*pi);

		if((*pi)->GetShadowEnabled())
		{
			render_point_light_shadows.push_back(*pi);
			(*pi)->SetCompareValue((light_pos - camera->GetPosition()).SquaredLen());
		}
	}

	if(point_light_shadow_limit == 0)
		render_point_light_shadows.clear();
	else if(point_light_shadow_limit > 0)
	{
		render_point_light_shadows.sort(CompareFloatComparable);

		while((int)render_point_light_shadows.size() > point_light_shadow_limit)
			render_point_light_shadows.pop_back();
	}


	for(pi=point_lights.begin(); pi!=point_lights.end(); pi++)
	{
		if(!(*pi)->GetShadowInvalid())
			continue;
		render_point_light_shadows.push_back(*pi);
	}
	render_point_light_shadows.unique();


	list<tPointLight *>::iterator pli;

	for(pli=render_point_light_shadows.begin(); pli!=render_point_light_shadows.end(); pli++)
	{
		light_pos = (*pli)->GetPosition();
		light_dist = (*pli)->GetDistance();

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

			(*pli)->GetShadow()->GetRenderSpace()->objects.insert((*i));
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




