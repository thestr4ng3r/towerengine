#ifndef _ENGINE_H
#define _ENGINE_H

class CEngine
{
private:
	static CDefaultFaceShader *default_face_shader;
	static CSkyBoxShader *skybox_shader;
	static CPointShadowShader *point_shadow_shader;
	static CPointShadowBlurShader *point_shadow_blur_shader;

	static CFaceShader *current_face_shader;

public:
	static void Init(void);
	static void Destroy(void);

	static CDefaultFaceShader *GetDefaultFaceShader(void)			{ return default_face_shader; }
	static CSkyBoxShader *GetSkyBoxShader(void)						{ return skybox_shader; }
	static CPointShadowShader *GetPointShadowShader(void)			{ return point_shadow_shader; }
	static CPointShadowBlurShader *GetPointShadowBlurShader(void)	{ return point_shadow_blur_shader; }

	static CFaceShader *GetCurrentFaceShader(void)		{ return current_face_shader; }
	static void SetCurrentFaceShader(CFaceShader *s)	{ current_face_shader = s; }
	static void BindCurrentFaceShader(void)				{ current_face_shader->Bind(); }
};

#endif
