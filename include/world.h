
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

	CVector ambient_color;

	void PutToGL(CVector cam_pos);
	void SetShadowTextureMatrix(void);

public:
	CWorld(void);
	~CWorld(void);

	void AddObject(CObject *o);
	void RemoveObject(CObject *o);
	void SetWorldMesh(CMesh *m, bool load_light = true);
	void SetSkyBox(CSkyBox *sky_box)	{ this->sky_box = sky_box; }
	void RemoveWorldMesh(void);
	void Clear(void);

	CVector GetAmbientColor(void)		{ return ambient_color; }
	CCamera *GetCamera(void)			{ return camera; }

	void SetAmbientColor(CVector v)		{ ambient_color = v; }

	void AddPointLight(CPointLight *light);
	void RemovePointLight(CPointLight *light);
	void ClearPointLights(void)			{ point_lights.clear(); }

	void AddDirectionalLight(CDirectionalLight *light);
	void RemoveDirectionalLight(CDirectionalLight *light);
	void ClearDirectionalLights(void);

	void RenderShadow(void);
	void RenderShadowMaps(void);
	void Render(int screen_width, int screen_height);
};


#endif
