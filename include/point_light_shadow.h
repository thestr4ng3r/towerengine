
#ifndef _POINT_LIGHT_SHADOW_H
#define _POINT_LIGHT_SHADOW_H


class tPointLightShadow
{
	private:
		tPointLight *light;

		tRenderSpace *render_space;

		GLuint tex;
		GLuint fbo;
		GLuint depth_rbo;

		bool blur_enabled;
		float blur_size;
		GLuint blur_tex;
		GLuint blur_fbo;
		static const GLenum blur_draw_buffers[6];

		tVAO *blur_vao;
		tVBO<float> *blur_vertex_vbo;

		int size;

		bool culled;

	public:
		tPointLightShadow(tPointLight *light, int size, bool blur_enabled = false);

		void Render(tRenderer *renderer);

		GLuint GetShadowMap(void)			{ return tex; }

		void SetBlurEnabled(bool enabled)	{ blur_enabled = enabled; }
		void SetBlurSize(float size)		{ blur_size = size; }
		void SetCulled(bool culled)			{ this->culled = culled; }

		bool GetBlurEnabled(void)			{ return blur_enabled; }
		float GetBlurSize(void)				{ return blur_size; }
		tRenderSpace *GetRenderSpace(void)	{ return render_space; }
		bool GetCulled(void)				{ return culled; }
};



#endif
