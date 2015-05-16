
#ifndef _WORLD_H
#define _WORLD_H

class tWorld
{
	private:
		vector<tObject *> objects;
		vector<tPointLight *> point_lights;
		vector<tDirectionalLight *> dir_lights;
		tSkyBox *sky_box;

		//tRenderSpace *camera_render_space;
		//set<tPointLight *> camera_render_point_lights;
		//set<tDirectionalLight *> camera_render_dir_lights;
		//list<tPointLight *> render_point_light_shadows;

		tVector ambient_color;

		struct physics
		{
			btBroadphaseInterface *broadphase;
			btDefaultCollisionConfiguration *collision_configuration;
			btCollisionDispatcher *collision_dispatcher;
			btSequentialImpulseConstraintSolver *solver;
			btDiscreteDynamicsWorld *dynamics_world;
		} physics;

	public:
		tWorld(void);
		~tWorld(void);

		void AddObject(tObject *o);
		void RemoveObject(tObject *o);
		void SetSkyBox(tSkyBox *sky_box)				{ this->sky_box = sky_box; }
		void Clear(void);

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
		void ClearDirectionalLights(void);

		int GetDirectionalLightsCount(void)				{ return dir_lights.size(); }
		tDirectionalLight *GetDirectionalLight(int i)	{ return dir_lights.at(i); }

		void Step(float time);

		void FillRenderObjectSpace(tRenderObjectSpace *space, tCulling *culling, bool clear = true, bool init_culling = true);
		void FillRenderSpace(tRenderSpace *space, tCulling *culling, bool init_culling = true);
		//tRenderSpace *GetCameraRenderSpace(void)		{ return camera_render_space; }
		tSkyBox *GetSkyBox(void)						{ return sky_box; }

		//set<tPointLight *> *GetCameraRenderPointLights(void)				{ return &camera_render_point_lights; }
		//set<tDirectionalLight *> *GetCameraRenderDirectionalLights(void)	{ return &camera_render_dir_lights; }

		//list<tPointLight *> *GetRenderPointLightShadows(void)				{ return &render_point_light_shadows; }
};


#endif
