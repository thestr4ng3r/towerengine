
#ifndef _RENDERER_H
#define _RENDERER_H


class tRenderer
{
	private:
		tWorld *world;

		tGBuffer *gbuffer;
		int screen_width, screen_height;

		GLuint fbo;
		GLuint color_tex;
		GLuint depth_tex;

		tVBO<float> *screen_quad_vbo;
		tVAO *screen_quad_vao;
		//tIBO *screen_quad_ibo;

		float projection_matrix[16];
		float modelview_matrix[16];

		bool fxaa_enabled;

		tSSAO *ssao;

		int point_light_shadow_limit;

		void GeometryPass(void);
		void LightPass(void);
		void ForwardPass(void);

	public:
		tRenderer(int width, int height, tWorld *world = 0);
		~tRenderer();

		void ChangeSize(int width, int height);

		void SetWorld(tWorld *world)	{ this->world = world; }

		void InitSSAO(int kernel_size, float radius, int noise_tex_size = 4);
		void SetFXAAEnabled(bool enabled)	{ fxaa_enabled = enabled; }

		void Render(GLuint dst_fbo = 0);
		void RenderScreenQuad(void);

		int GetScreenWidth(void)	{ return screen_width; }
		int GetScreenHeight(void)	{ return screen_height; }

		tWorld *GetWorld(void)		{ return world; }

		float *GetProjectionMatrix(void)	{ return projection_matrix; }
		float *GetModelViewMatrix(void)		{ return modelview_matrix; }

		tGBuffer *GetGBuffer(void)			{ return gbuffer; }

		GLuint GetDepthTexture(void)		{ return depth_tex; }

		void SetPointLightShadowRenderLimit(int limit)	{ this->point_light_shadow_limit = limit; }
};


#endif
