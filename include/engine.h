#ifndef _ENGINE_H
#define _ENGINE_H

class tEngine
{
private:
	static tGeometryPassShader *geometry_pass_shader;
	static tLightPassShader *light_pass_shader;
	static tAmbientLightingShader *ambient_lighting_shader;
	static tDirectionalLightingShader *directional_lighting_shader;

	static tSkyBoxShader *skybox_shader;
	static tPointShadowShader *point_shadow_shader;
	static tPointShadowBlurShader *point_shadow_blur_shader;
	static tDirectionalShadowShader *directional_shadow_shader;
	static tDirectionalShadowBlurShader *directional_shadow_blur_shader;
	static tColorShader *color_shader;
	static tPostProcessShader *post_process_shader;
	static tSSAOShader *ssao_shader;

	static tFaceShader *current_face_shader;

public:
	static const float identity_matrix4[16];

	static void Init(void);
	static void Destroy(void);

	static tGeometryPassShader *GetGeometryPassShader(void)						{ return geometry_pass_shader; }
	static tLightPassShader *GetLightPassShader(void)							{ return light_pass_shader; }
	static tDirectionalLightingShader *GetDirectionalLightingShader(void)		{ return directional_lighting_shader; }
	static tAmbientLightingShader *GetAmbientLightingShader(void)				{ return ambient_lighting_shader; }
	static tSkyBoxShader *GetSkyBoxShader(void)									{ return skybox_shader; }
	static tPointShadowShader *GetPointShadowShader(void)						{ return point_shadow_shader; }
	static tPointShadowBlurShader *GetPointShadowBlurShader(void)				{ return point_shadow_blur_shader; }
	static tDirectionalShadowShader *GetDirectionalShadowShader(void)			{ return directional_shadow_shader; }
	static tDirectionalShadowBlurShader *GetDirectionalShadowBlurShader(void)	{ return directional_shadow_blur_shader; }
	static tColorShader *GetColorShader(void)									{ return color_shader; }
	static tPostProcessShader *GetPostProcessShader(void)						{ return post_process_shader; }
	static tSSAOShader *GetSSAOShader(void)										{ return ssao_shader; }

	static tFaceShader *GetCurrentFaceShader(void)		{ return current_face_shader; }
	static void SetCurrentFaceShader(tFaceShader *s)	{ current_face_shader = s; }
	static void BindCurrentFaceShader(void)				{ current_face_shader->Bind(); }
};

#endif
