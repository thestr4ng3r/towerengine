
#ifndef _WORLD_H
#define _WORLD_H

class CWorld
{
private:
	vector<CObject *> objects;
	vector<CPointLight *> point_lights;
	vector<CDirectionalLight *> dir_lights;
	CMeshObject *world_object;
	CSkyBox *sky_box;
	CCamera *camera;

	CGBuffer *gbuffer;
	int screen_width, screen_height;

	CRenderSpace *camera_render_space;
	set<CPointLight *> camera_render_point_lights;
	set<CDirectionalLight *> camera_render_dir_lights;

	struct render
	{
		GLuint fbo;
		GLuint tex;
		GLuint depth_rbo;

		float *point_light_pos, *point_light_color, *point_light_distance;
		int *point_light_shadow_enabled;
		GLuint *point_light_shadow_maps;

		float *dir_light_dir, *dir_light_color, *dir_light_shadow_clip, *dir_light_shadow_tex_matrix;
		float *dir_light_shadow_splits_count, *dir_light_shadow_splits_z;
		int *dir_light_shadow_enabled;
		GLuint *dir_light_shadow_maps;
	} render;

	CVector ambient_color;

	struct physics
	{
		btBroadphaseInterface *broadphase;
		btDefaultCollisionConfiguration *collision_configuration;
		btCollisionDispatcher *collision_dispatcher;
		btSequentialImpulseConstraintSolver *solver;
		btDiscreteDynamicsWorld *dynamics_world;
	} physics;

	//void PaintObjects(void);
	void FillRenderSpaces(void);
	void RenderShadowMaps(void);

	void GeometryPass(void);
	void LightPass(void);

public:
	CWorld(int width, int height);
	~CWorld(void);

	void AddObject(CObject *o);
	void RemoveObject(CObject *o);
	void SetWorldMesh(CMesh *m, bool load_light = true);
	void SetSkyBox(CSkyBox *sky_box)	{ this->sky_box = sky_box; }
	void RemoveWorldMesh(void);
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
	void Render(void);
};


#endif
