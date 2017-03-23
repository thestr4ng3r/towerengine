
#ifndef _TOWERENGINE_RENDERER_H
#define _TOWERENGINE_RENDERER_H

class tRenderer
{
	private:
		void InitShaders();

		void RenderShadowMaps(void);

	protected:
		tWorld *world;

		tPointShadowShader *point_shadow_shader;
		tPointShadowBlurShader *point_shadow_blur_shader;
		tDirectionalShadowShader *directional_shadow_shader;
		tDirectionalShadowBlurShader *directional_shadow_blur_shader;

		tDepthPassShader *depth_pass_shader;

		bool shadow_pass;

		tCamera *current_rendering_camera;
		tRenderSpace *current_rendering_render_space;

		tFaceShader *current_face_shader;

		int point_light_shadow_limit;
		std::list<tPointLight *> render_point_light_shadows;

		tVBO<float> *screen_quad_vbo;
		tVBO<float> *screen_quad_uv_vbo;
		tVAO *screen_quad_vao;

		void InitRenderer(tWorld *world);
		virtual void PrepareRender(tCamera *camera, tRenderSpace *render_space);

	public:
		virtual ~tRenderer();

		tPointShadowShader *GetPointShadowShader(void)						{ return point_shadow_shader; }
		tPointShadowBlurShader *GetPointShadowBlurShader(void)				{ return point_shadow_blur_shader; }
		tDirectionalShadowShader *GetDirectionalShadowShader(void)			{ return directional_shadow_shader; }
		tDirectionalShadowBlurShader *GetDirectionalShadowBlurShader(void)	{ return directional_shadow_blur_shader; }
		tDepthPassShader *GetDepthPassShader(void)							{ return depth_pass_shader; }

		bool GetShadowPass(void)					{ return shadow_pass; }
		tCamera *GetCurrentRenderingCamera(void)	{ return current_rendering_camera; }

		tFaceShader *GetCurrentFaceShader(void)		{ return current_face_shader; }
		void BindCurrentFaceShader(void)			{ current_face_shader->Bind(); }
		void SetCurrentFaceShader(tFaceShader *s)	{ current_face_shader = s; }

		void SetPointLightShadowRenderLimit(int limit)	{ this->point_light_shadow_limit = limit; }


		void SetWorld(tWorld *world)	{ this->world = world; }
		tWorld *GetWorld(void)		{ return world; }

		void RenderScreenQuad(void);
};

#endif
