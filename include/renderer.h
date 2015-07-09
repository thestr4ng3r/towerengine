
#ifndef _RENDERER_H
#define _RENDERER_H


class tRenderer
{
	private:
		tGeometryPassShader *geometry_pass_shader;
		tAmbientLightingShader *ambient_lighting_shader;
		tSSAOAmbientLightingShader *ssao_ambient_lighting_shader;
		tDirectionalLightingShader *directional_lighting_shader;
		std::vector<tPointLightingShader *> point_lighting_shaders;
		tSSAOLightingShader *ssao_lighting_shader;

		tSkyBoxShader *skybox_shader;
		tPointShadowShader *point_shadow_shader;
		tPointShadowBlurShader *point_shadow_blur_shader;
		tDirectionalShadowShader *directional_shadow_shader;
		tDirectionalShadowBlurShader *directional_shadow_blur_shader;
		tColorShader *color_shader;
		tPostProcessShader *post_process_shader;

		tParticleShader *particle_shader;

		tSSAOShader *ssao_shader;

		tFogShader *fog_shader;
		bool fog_enabled;
		float fog_start, fog_end, fog_exp;

		tFaceShader *current_face_shader;

		tWorld *world;

		tGBuffer *gbuffer;
		int screen_width, screen_height;

		GLuint fbo;
		GLuint *color_tex;
		GLuint depth_tex;

		tVBO<float> *screen_quad_vbo;
		tVAO *screen_quad_vao;
		//tIBO *screen_quad_ibo;

		bool fxaa_enabled;

		tSSAO *ssao;
		bool ssao_ambient_only;

		int point_light_shadow_limit;

		tCamera *camera;
		tRenderSpace *camera_render_space;
		std::list<tPointLight *> render_point_light_shadows;


		tCubeMapReflection *cube_map_reflection;


		void RenderShadowMaps(void);
		void GeometryPass(void);
		void LightPass(void);
		void ForwardPass(void);

	public:
		tRenderer(int width, int height, tWorld *world = 0);
		~tRenderer();

		void ChangeSize(int width, int height);

		void SetWorld(tWorld *world)	{ this->world = world; }

		void InitSSAO(bool ambient_only, int kernel_size, float radius, int noise_tex_size = 4);
		void SetFXAAEnabled(bool enabled)	{ fxaa_enabled = enabled; }

		void SetFog(bool enabled, float start_dist = 0.0, float end_dist = 100.0, float exp = 1.0, tVector color = Vec(0.0, 0.0, 0.0));

		void InitCubeMapReflection(int resolution, tVector position);

		void Render(GLuint dst_fbo = 0, int width = 0, int height = 0);
		void RenderScreenQuad(void);

		tFaceShader *GetCurrentFaceShader(void)		{ return current_face_shader; }
		void BindCurrentFaceShader(void)			{ current_face_shader->Bind(); }
		void SetCurrentFaceShader(tFaceShader *s)	{ current_face_shader = s; }

		tGeometryPassShader *GetGeometryPassShader(void)					{ return geometry_pass_shader; }
		tAmbientLightingShader *GetAmbientLightingShader(void)				{ return ambient_lighting_shader; }
		tDirectionalLightingShader *GetDirectionalLightingShader(void)		{ return directional_lighting_shader; }
		//tPointLightingShader *GetPointLightingShader(void)					{ return point_lighting_shader; }
		std::vector<tPointLightingShader *>::iterator GetPointLightingShadersBegin(void)	{ return point_lighting_shaders.begin(); }
		std::vector<tPointLightingShader *>::iterator GetPointLightingShadersEnd(void)		{ return point_lighting_shaders.end(); }
		tPointShadowShader *GetPointShadowShader(void)						{ return point_shadow_shader; }
		tPointShadowBlurShader *GetPointShadowBlurShader(void)				{ return point_shadow_blur_shader; }
		tDirectionalShadowShader *GetDirectionalShadowShader(void)			{ return directional_shadow_shader; }
		tDirectionalShadowBlurShader *GetDirectionalShadowBlurShader(void)	{ return directional_shadow_blur_shader; }
		tSkyBoxShader *GetSkyBoxShader(void)								{ return skybox_shader; }
		tColorShader *GetColorShader(void)									{ return color_shader; }
		tSSAOShader *GetSSAOShader(void)									{ return ssao_shader; }
		tParticleShader *GetParticleShader(void)							{ return particle_shader; }

		int GetScreenWidth(void)	{ return screen_width; }
		int GetScreenHeight(void)	{ return screen_height; }

		tWorld *GetWorld(void)		{ return world; }
		tCamera *GetCamera(void)			{ return camera; }

		//float *GetProjectionMatrix(void)	{ return projection_matrix; }
		//float *GetModelViewMatrix(void)		{ return modelview_matrix; }

		tGBuffer *GetGBuffer(void)			{ return gbuffer; }

		GLuint GetDepthTexture(void)		{ return depth_tex; }

		tCubeMapReflection *GetCubeMapReflection(void)	{ return cube_map_reflection; }

		void SetPointLightShadowRenderLimit(int limit)	{ this->point_light_shadow_limit = limit; }
};


#endif
