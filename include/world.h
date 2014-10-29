
#ifndef _WORLD_H
#define _WORLD_H

class CWorld
{
private:
	vector<CObject *> objects;
	vector<CPointLight *> point_lights;
	vector<CDirectionalLight *> dir_lights;
	CSkyBox *sky_box;
	CCamera *camera;

	CRenderSpace *camera_render_space;
	set<CPointLight *> camera_render_point_lights;
	set<CDirectionalLight *> camera_render_dir_lights;

	CVector ambient_color;

	struct physics
	{
		btBroadphaseInterface *broadphase;
		btDefaultCollisionConfiguration *collision_configuration;
		btCollisionDispatcher *collision_dispatcher;
		btSequentialImpulseConstraintSolver *solver;
		btDiscreteDynamicsWorld *dynamics_world;
	} physics;

public:
	CWorld(void);
	~CWorld(void);

	void AddObject(CObject *o);
	void RemoveObject(CObject *o);
	void SetSkyBox(CSkyBox *sky_box)	{ this->sky_box = sky_box; }
	void Clear(void);

	CVector GetAmbientColor(void)		{ return ambient_color; }
	CCamera *GetCamera(void)			{ return camera; }
	btDiscreteDynamicsWorld *GetDynamicsWorld(void) { return physics.dynamics_world; }
	btBroadphaseInterface *GetBroadphase(void)		{ return physics.broadphase; }

	void SetAmbientColor(CVector v)		{ ambient_color = v; }

	void AddPointLight(CPointLight *light);
	void RemovePointLight(CPointLight *light);
	void ClearPointLights(void)			{ point_lights.clear(); }

	void AddDirectionalLight(CDirectionalLight *light);
	void RemoveDirectionalLight(CDirectionalLight *light);
	void ClearDirectionalLights(void);

	void Step(float time);

	void GetPointLightUniforms(int &count, float *pos, float *color, float *distance, int *shadow_enabled, GLuint *shadow_maps);
	void GetDirectionalLightUniforms(int &count, float *dir, float *color, float *shadow_clip, float *shadow_tex_matrix, float *shadow_splits_count, float *shadow_splits_z, int *shadow_enabled, GLuint *shadow_maps);

	void FillRenderSpaces(void);
	void RenderShadowMaps(void);
	CRenderSpace *GetCameraRenderSpace(void)	{ return camera_render_space; }
	CSkyBox *GetSkyBox(void)					{ return sky_box; }
};


#endif
