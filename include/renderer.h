
#ifndef _RENDERER_H
#define _RENDERER_H


class tRenderer
{
	protected:
		tGeometryPassShader *geometry_pass_shader;
		tDepthPassShader *depth_pass_shader;
		tSimpleForwardShader *simple_forward_shader;
		tRefractionShader *refraction_shader;
#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
		tLightingShader *lighting_shader;
#endif
		tAmbientLightingShader *ambient_lighting_shader;
		tAmbientLightingShader *ssao_ambient_lighting_shader;
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

		tParticleForwardShader *particle_forward_shader;
		tParticleDeferredShader *particle_deferred_shader;

		tSSAOShader *ssao_shader;

		tCubeMapBlurShader *cube_map_blur_shader;

		tFogShader *fog_shader;
		bool fog_enabled;
		float fog_start, fog_end, fog_exp;

		tFaceShader *current_face_shader;
		bool shadow_pass;

		int screen_width, screen_height;

		tWorld *world;

		tGBuffer *gbuffer;

		GLuint fbo;
		GLuint *color_tex;

		tVBO<float> *screen_quad_vbo;
		tVAO *screen_quad_vao;

		bool fxaa_enabled;

		tSSAO *ssao;
		bool ssao_ambient_only;

		bool depth_prepass_enabled;

		bool bindless_textures_enabled;

		int point_light_shadow_limit;

		
		std::list<tPointLight *> render_point_light_shadows;
#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
		tLightingShaderPointLightsBuffer *point_lights_buffer;
#endif

		tMatrixBuffer *matrix_buffer;
		tPositionRestoreDataBuffer *position_restore_data_buffer;

		int current_read_color_tex;

		tCamera *current_rendering_camera;
		tRenderSpace *current_rendering_render_space;

		void InitRenderer(int width, int height, tWorld *world, bool bindless_textures);
		void PrepareRender(tCamera *camera, tRenderSpace *render_space);
		void Render(tCamera *camera, tRenderSpace *render_space, GLuint dst_fbo, int viewport_x, int viewport_y, int viewport_width, int viewport_height);

		void RenderShadowMaps(void);
		void DepthPrePass(void);
		void GeometryPass(void);
		void LightPass(void);
		void ForwardPass(void);
		void RefractionPass(void);

		void SetReflections(tReflectingShader *shader, tVector pos);

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
		void BindlessTexturesLightPass(void);
#endif
		void LegacyLightPass(void);

		void ChangeScreenSize(int width, int height);

	public:
		virtual ~tRenderer();


		void SetWorld(tWorld *world)	{ this->world = world; }

		void InitSSAO(bool ambient_only, int kernel_size, float radius, int noise_tex_size = 4);
		void SetFXAAEnabled(bool enabled)	{ fxaa_enabled = enabled; }

		void SetFog(bool enabled, float start_dist = 0.0, float end_dist = 100.0, float exp = 1.0, tVector color = Vec(0.0, 0.0, 0.0));

		//void InitCubeMapReflection(int resolution, tVector position);

		void RenderScreenQuad(void);

		tFaceShader *GetCurrentFaceShader(void)		{ return current_face_shader; }
		void BindCurrentFaceShader(void)			{ current_face_shader->Bind(); }
		void SetCurrentFaceShader(tFaceShader *s)	{ current_face_shader = s; }
		bool GetShadowPass(void)					{ return shadow_pass; }

		tGeometryPassShader *GetGeometryPassShader(void)					{ return geometry_pass_shader; }
		tSimpleForwardShader *GetSimpleForwardShader(void)					{ return simple_forward_shader; }
		tRefractionShader *GetRefractionShader(void)						{ return refraction_shader; }
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
		//tParticleForwardShader *GetParticleShader(void)							{ return particle_shader; }
		tDepthPassShader *GetDepthPassShader(void)							{ return depth_pass_shader; }
		tCubeMapBlurShader *GetCubeMapBlurShader(void)						{ return cube_map_blur_shader; }

		int GetScreenWidth(void)	{ return screen_width; }
		int GetScreenHeight(void)	{ return screen_height; }

		tWorld *GetWorld(void)		{ return world; }

		//float *GetProjectionMatrix(void)	{ return projection_matrix; }
		//float *GetModelViewMatrix(void)		{ return modelview_matrix; }

		tCamera *GetCurrentRenderingCamera(void)	{ return current_rendering_camera; }

		tGBuffer *GetGBuffer(void)			{ return gbuffer; }

		//GLuint GetDepthTexture(void)		{ return depth_tex; }
		GLuint GetCurrentReadColorTexture(void)			{ return color_tex[current_read_color_tex]; }
		//tCubeMapReflection *GetCubeMapReflection(void)	{ return cube_map_reflection; }

		void SetPointLightShadowRenderLimit(int limit)	{ this->point_light_shadow_limit = limit; }

		void SetDepthPrePassEnabled(bool enabled)		{ this->depth_prepass_enabled = enabled; }
		bool GetDepthPrePassEnabled(void)				{ return depth_prepass_enabled; }

		tMatrixBuffer *GetMatrixBuffer(void)							{ return matrix_buffer; }
		tPositionRestoreDataBuffer *GetPositionRestoreDataBuffer(void)	{ return position_restore_data_buffer; }
};


#endif
