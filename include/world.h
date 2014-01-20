
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

	CVector ambient_color;

	struct shadow
	{
		bool enabled;

		GLuint tex;
		GLuint fbo;
		GLuint rbo;

		int width;
		int height;
	} shadow;

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

	void InitShadow(int width, int height);
	void SetShadowEnabled(bool enabled)	{ shadow.enabled = enabled; }

	CVector GetAmbientColor(void)		{ return ambient_color; }

	void SetAmbientColor(CVector v)		{ ambient_color = v; }

	void AddPointLight(CPointLight *light);
	void RemovePointLight(CPointLight *light);
	void ClearPointLights(void)			{ point_lights.clear(); }

	void AddDirectionalLight(CDirectionalLight *light);
	void RemoveDirectionalLight(CDirectionalLight *light);
	void ClearDirectionalLights(void);

	void RenderShadow(CVector cam_pos);
	void Render(CVector cam_pos);
};


#endif
