
#ifndef _DEFERRED_RENDERER_H
#define _DEFERRED_RENDERER_H


class tDeferredRenderer : public tRenderer
{
	private:
		void InitShaders(void);

		void DepthPrePass(void);

	protected:
		tGeometryPassShader *geometry_pass_shader;
		tRefractionShader *refraction_shader;
#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
		tLightingShader *lighting_shader;
#endif
		tAmbientLightingShader *ambient_lighting_shader;
		tAmbientLightingShader *ssao_ambient_lighting_shader;
		tDirectionalLightingShader *directional_lighting_shader;
		std::vector<tPointLightingShader *> point_lighting_shaders;
		tSSAOLightingShader *ssao_lighting_shader;

		tPostProcessShader *post_process_shader;

		tParticleForwardShader *particle_forward_shader;
		tParticleDeferredShader *particle_deferred_shader;

		tSSAOShader *ssao_shader;

		tCubeMapBlurShader *cube_map_blur_shader;

		tFogShader *fog_shader;
		bool fog_enabled;
		float fog_start, fog_end, fog_exp;

		int screen_width, screen_height;


		tGBuffer *gbuffer;

		GLuint fbo;
		GLuint *color_tex;

		bool fxaa_enabled;

		tSSAO *ssao;
		bool ssao_ambient_only;


		bool bindless_textures_enabled;



#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
		tLightingShaderPointLightsBuffer *point_lights_buffer;
#endif

		tPositionRestoreDataBuffer *position_restore_data_buffer;

		int current_read_color_tex;


		void InitDeferredRenderer(int width, int height, tWorld *world, bool bindless_textures);

		void GeometryPass(void);
		void LightPass(void);
		void ForwardPass(void);
		void RefractionPass(void);

		void PrepareRender(tCamera *camera, tRenderObjectSpace *render_space);
		void RenderCubeMapReflections(void);

		void SetReflections(tReflectingShader *shader, tVector pos);

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
		void BindlessTexturesLightPass(void);
#endif
		void LegacyLightPass(void);

		void ChangeScreenSize(int width, int height);

	public:
		virtual ~tDeferredRenderer();



		void InitSSAO(bool ambient_only, int kernel_size, float radius, int noise_tex_size = 4);
		void SetFXAAEnabled(bool enabled)	{ fxaa_enabled = enabled; }

		void SetFog(bool enabled, float start_dist = 0.0, float end_dist = 100.0, float exp = 1.0, tVector color = tVec(0.0, 0.0, 0.0));

		tGeometryPassShader *GetGeometryPassShader(void)					{ return geometry_pass_shader; }
		tRefractionShader *GetRefractionShader(void)						{ return refraction_shader; }
		tAmbientLightingShader *GetAmbientLightingShader(void)				{ return ambient_lighting_shader; }
		tDirectionalLightingShader *GetDirectionalLightingShader(void)		{ return directional_lighting_shader; }
		std::vector<tPointLightingShader *>::iterator GetPointLightingShadersBegin(void)	{ return point_lighting_shaders.begin(); }
		std::vector<tPointLightingShader *>::iterator GetPointLightingShadersEnd(void)		{ return point_lighting_shaders.end(); }
		tSSAOShader *GetSSAOShader(void)									{ return ssao_shader; }
		tCubeMapBlurShader *GetCubeMapBlurShader(void)						{ return cube_map_blur_shader; }

		int GetScreenWidth(void)	{ return screen_width; }
		int GetScreenHeight(void)	{ return screen_height; }


		tGBuffer *GetGBuffer(void)			{ return gbuffer; }

		GLuint GetCurrentReadColorTexture(void)			{ return color_tex[current_read_color_tex]; }


		tPositionRestoreDataBuffer *GetPositionRestoreDataBuffer(void)	{ return position_restore_data_buffer; }
};


#endif
