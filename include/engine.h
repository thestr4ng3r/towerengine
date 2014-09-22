#ifndef _ENGINE_H
#define _ENGINE_H

class CEngine
{
private:
	static CGeometryPassShader *geometry_pass_shader;
	static CLightPassShader *light_pass_shader;
	static CSkyBoxShader *skybox_shader;
	static CPointShadowShader *point_shadow_shader;
	static CPointShadowBlurShader *point_shadow_blur_shader;
	static CDirectionalShadowShader *directional_shadow_shader;
	static CDirectionalShadowBlurShader *directional_shadow_blur_shader;
	static CPostProcessShader *post_process_shader;
	static CSSAOShader *ssao_shader;

	static CFaceShader *current_face_shader;

public:
	static const float identity_matrix4[16];

	static void Init(void);
	static void Destroy(void);

	static CGeometryPassShader *GetGeometryPassShader(void)						{ return geometry_pass_shader; }
	static CLightPassShader *GetLightPassShader(void)							{ return light_pass_shader; }
	static CSkyBoxShader *GetSkyBoxShader(void)									{ return skybox_shader; }
	static CPointShadowShader *GetPointShadowShader(void)						{ return point_shadow_shader; }
	static CPointShadowBlurShader *GetPointShadowBlurShader(void)				{ return point_shadow_blur_shader; }
	static CDirectionalShadowShader *GetDirectionalShadowShader(void)			{ return directional_shadow_shader; }
	static CDirectionalShadowBlurShader *GetDirectionalShadowBlurShader(void)	{ return directional_shadow_blur_shader; }
	static CPostProcessShader *GetPostProcessShader(void)						{ return post_process_shader; }
	static CSSAOShader *GetSSAOShader(void)										{ return ssao_shader; }

	static CFaceShader *GetCurrentFaceShader(void)		{ return current_face_shader; }
	static void SetCurrentFaceShader(CFaceShader *s)	{ current_face_shader = s; }
	static void BindCurrentFaceShader(void)				{ current_face_shader->Bind(); }
};

#endif
