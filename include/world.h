
#ifndef _WORLD_H
#define _WORLD_H

class CWorld
{
private:
	vector<CObject *> objects;
	CMeshObject *world_object;

	CVector light_pos;
	CVector light_color;
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
	void RemoveWorldMesh(void);
	void Clear(void);

	void InitShadow(int width, int height);
	void SetShadowEnabled(bool enabled)	{ shadow.enabled = enabled; }

	CVector GetLightPos(void)			{ return light_pos; }
	CVector GetLightColor(void)		{ return light_color; }
	CVector GetAmbientColor(void)		{ return ambient_color; }

	void SetLightPos(CVector v)		{ light_pos = v; }
	void SetLightColor(CVector v)		{ light_color = v; }
	void SetAmbientColor(CVector v)	{ ambient_color = v; }

	void RenderShadow(CVector cam_pos);
	void Render(CVector cam_pos);
};


#endif
