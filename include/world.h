
#ifndef _WORLD_H
#define _WORLD_H

#include <unordered_set>

class tWorld
{
	private:
		std::vector<tObject *> objects;
		tSkyBox *sky_box;

		tVector ambient_color;

		std::unordered_set<tReflectionProbe *> reflection_probes;
		//std::unordered_set<tParticleSystem *> particle_systems;

		struct physics
		{
			btBroadphaseInterface *broadphase;
			btDefaultCollisionConfiguration *collision_configuration;
			btCollisionDispatcher *collision_dispatcher;
			btSequentialImpulseConstraintSolver *solver;
			btDiscreteDynamicsWorld *dynamics_world;
			btOverlappingPairCallback *ghost_pair_callback;
		} physics;

	public:
		tWorld(void);
		~tWorld(void);

		void AddObject(tObject *o);
		void RemoveObject(tObject *o);
		const std::vector<tObject *> &GetObjects() const							{ return objects; }

		const std::unordered_set<tReflectionProbe *> &GetReflectionProbes() const	{ return reflection_probes; }
		//const std::unordered_set<tParticleSystem *> &GetParticleSystems() const 	{ return particle_systems; }

		void SetSkyBox(tSkyBox *sky_box)				{ this->sky_box = sky_box; }
		void ClearObjects(void)							{ objects.clear(); }

		tVector GetAmbientColor(void)					{ return ambient_color; }
		btDiscreteDynamicsWorld *GetDynamicsWorld(void) { return physics.dynamics_world; }
		btBroadphaseInterface *GetBroadphase(void)		{ return physics.broadphase; }

		void SetAmbientColor(tVector v)					{ ambient_color = v; }

		void Step(float time, int max_sub_steps = 1, float fixed_time_step = 1.0f / 60.0f);

		void FillObjectSpace(tObjectSpace *space, tCulling **cullings, int cullings_count, bool clear = true, bool init_cullings = true);
		//void FillRenderSpace(tRenderObjectSpace *space, tCulling **cullings, int cullings_count, bool init_cullings = true);

		tSkyBox *GetSkyBox(void)						{ return sky_box; }

		void AssignUnsetReflectionProbes(void);
};


#endif
