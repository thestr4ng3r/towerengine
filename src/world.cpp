
#include "towerengine.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"

using namespace std;

tWorld::tWorld(void)
{
	ambient_color = tVec(0.1, 0.1, 0.1);
	sky_box = 0;

	physics.broadphase = new btDbvtBroadphase();
	physics.collision_configuration = new btDefaultCollisionConfiguration();
	physics.collision_dispatcher = new btCollisionDispatcher(physics.collision_configuration);
	physics.solver = new btSequentialImpulseConstraintSolver();
	physics.dynamics_world = new btDiscreteDynamicsWorld(physics.collision_dispatcher, physics.broadphase, physics.solver, physics.collision_configuration);
	physics.dynamics_world->setGravity(btVector3(0, -10, 0));
	//physics.dynamics_world->getDispatchInfo().m_allowedCcdPenetration=0.0001f;

	physics.ghost_pair_callback = new btGhostPairCallback();
	physics.broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(physics.ghost_pair_callback);
}

tWorld::~tWorld(void)
{
	delete physics.dynamics_world;
	delete physics.solver;
	delete physics.collision_dispatcher;
	delete physics.collision_configuration;
	delete physics.broadphase;
	delete physics.ghost_pair_callback;

	/*for(vector<tObject *>::iterator i=objects.begin(); i!=objects.end(); i++)
		delete *i;

	for(vector<tPointLight *>::iterator i=point_lights.begin(); i!=point_lights.end(); i++)
		delete *i;

	for(vector<tDirectionalLight *>::iterator i=dir_lights.begin(); i!=dir_lights.end(); i++)
		delete *i;

	for(vector<tParticleSystem *>::iterator i=particle_systems.begin(); i!=particle_systems.end(); i++)
		delete *i;

	for(vector<tReflectionProbe *>::iterator i=reflection_probes.begin(); i!=reflection_probes.end(); i++)
		delete *i;*/
}

void tWorld::AddObject(tObject *o)
{
	vector<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(*i == o)
			return;

	objects.push_back(o);

	if(tReflectionProbe *reflection_probe = dynamic_cast<tReflectionProbe *>(o))
		reflection_probes.insert(reflection_probe);

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

	if(tReflectionProbe *reflection_probe = dynamic_cast<tReflectionProbe *>(o))
		reflection_probes.erase(reflection_probe);
}

void tWorld::FillObjectSpace(tObjectSpace *space, tCulling *culling, bool clear, bool init_culling)
{
	if(clear)
		space->Clear();

	if(init_culling)
		culling->InitCulling();

	for(tObject *object : objects)
	{
		if(!object->GetEnabled())
			continue;

		tAABB aabb = object->GetAABB();
		if(culling->TestAABBCulling(aabb))
			continue;

		space->AddObject(object);
	}
}


void tWorld::AssignUnsetReflectionProbes(void)
{
	for(vector<tObject *>::iterator i=objects.begin(); i!=objects.end(); i++)
	{
		tMeshObject *mesh_object = dynamic_cast<tMeshObject *>(*i);
		if(!mesh_object)
			continue;

		if(mesh_object->GetCubeMapReflection() // Reflection already set
				|| !mesh_object->GetCubeMapReflectionEnabled()) // or not needed
			continue;

		tReflectionProbe *min = 0;
		float min_v = FLT_MAX;

		for(auto ri=reflection_probes.begin(); ri!=reflection_probes.end(); ri++)
		{
			tReflectionProbe *reflection = *ri;
			float v = (reflection->GetPosition() - mesh_object->GetTransform().GetPosition()).SquaredLen();
			if(v < min_v)
			{
				min_v = v;
				min = reflection;
			}
		}

		if(min)
			mesh_object->SetCubeMapReflection(min);
	}
}


void tWorld::Step(float time, int max_sub_steps, float fixed_time_step)
{
	if(time < 0.001)
		time = 0.001;

	physics.dynamics_world->stepSimulation(time, max_sub_steps, fixed_time_step);
	//CProfileManager::dumpAll();

	//for(vector<tParticleSystem *>::iterator psi = particle_systems.begin(); psi != particle_systems.end(); psi++)
	//	(*psi)->Step(time);
}




