
#ifndef _WORLD_H
#define _WORLD_H

class tWorld
{
	private:
		std::vector<tObject *> objects;
		std::vector<tPointLight *> point_lights;
		std::vector<tDirectionalLight *> dir_lights;
		std::vector<tParticleSystem *> particle_systems;
		std::vector<tCubeMapReflection *> cube_map_reflections;
		tSkyBox *sky_box;

		tVector ambient_color;

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
		void SetSkyBox(tSkyBox *sky_box)				{ this->sky_box = sky_box; }
		void ClearObjects(void)							{ objects.clear(); }

		tVector GetAmbientColor(void)					{ return ambient_color; }
		btDiscreteDynamicsWorld *GetDynamicsWorld(void) { return physics.dynamics_world; }
		btBroadphaseInterface *GetBroadphase(void)		{ return physics.broadphase; }

		void SetAmbientColor(tVector v)					{ ambient_color = v; }

		void AddPointLight(tPointLight *light);
		void RemovePointLight(tPointLight *light);
		void ClearPointLights(void)						{ point_lights.clear(); }

		int GetPointLightsCount(void)					{ return point_lights.size(); }
		tPointLight *GetPointLight(int i)				{ return point_lights.at(i); }

		void AddDirectionalLight(tDirectionalLight *light);
		void RemoveDirectionalLight(tDirectionalLight *light);
		void ClearDirectionalLights(void)				{ dir_lights.clear(); }

		int GetDirectionalLightsCount(void)				{ return dir_lights.size(); }
		tDirectionalLight *GetDirectionalLight(int i)	{ return dir_lights.at(i); }

		void AddParticleSystem(tParticleSystem *ps);
		void RemoveParticleSystem(tParticleSystem *ps);
		void ClearParticleSystems(void)					{ particle_systems.clear(); }

		int GetParticleSystemsCount(void)				{ return particle_systems.size(); }
		tParticleSystem *GetParticleSystem(int i)		{ return particle_systems.at(i); }

		void AddCubeMapReflection(tCubeMapReflection *r);
		void RemoveCubeMapReflection(tCubeMapReflection *r);

		int GetCubeMapReflectionsCount(void)			{ return cube_map_reflections.size(); }
		tCubeMapReflection *GetCubeMapReflection(int i)	{ return cube_map_reflections.at(i); }

		void Step(float time, int max_sub_steps = 1, float fixed_time_step = 1.0f / 60.0f);

		void FillRenderObjectSpace(tRenderObjectSpace *space, tCulling **cullings, int cullings_count, bool clear = true, bool init_cullings = true);
		void FillRenderSpace(tRenderSpace *space, tCulling **cullings, int cullings_count, bool init_cullings = true);

		tSkyBox *GetSkyBox(void)						{ return sky_box; }

		void AssignUnsetCubeMapReflections(void);
};


#endif
