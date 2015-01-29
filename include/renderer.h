
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

		struct ssao
		{
			bool enabled;
			float *kernel;
			int kernel_size;
			GLuint noise_tex;
			int noise_tex_size;
			float radius;

			GLuint tex;
			GLenum draw_buffer;
		} ssao;

		bool fxaa_enabled;

		void GeometryPass(void);
		void LightPass(void);
		void ForwardPass(void);

		void RenderSSAO(void);

	public:
		tRenderer(int width, int height, tWorld *world = 0);
		~tRenderer();

		void ChangeSize(int width, int height);

		void SetWorld(tWorld *world)	{ this->world = world; }

		void InitSSAO(int kernel_size, float radius, int noise_tex_size = 4);
		void SetFXAAEnabled(bool enabled)	{ fxaa_enabled = enabled; }

		void Render(void);


		void RenderLightingScreenQuad(void);
};


#endif
