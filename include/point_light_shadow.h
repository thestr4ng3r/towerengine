
#ifndef _POINT_LIGHT_SHADOW_H
#define _POINT_LIGHT_SHADOW_H


class CPointLightShadow
{
	private:
		CPointLight *light;

		CRenderSpace *render_space;

		GLuint tex;
		GLuint fbo;
		GLuint depth_rbo;

		bool blur_enabled;
		float blur_size;
		GLuint blur_tex;
		GLuint blur_fbo;
		static const GLenum blur_draw_buffers[6];

		VAO *blur_vao;
		VBO<float> *blur_vertex_vbo;

		int size;

		bool culled;

	public:
		CPointLightShadow(CPointLight *light, int size, bool blur_enabled = false, float blur_size = 0.003);

		void Render(CWorld *world);

		GLuint GetShadowMap(void)			{ return tex; }

		void SetBlurEnabled(bool enabled)	{ blur_enabled = enabled; }
		void SetBlurSize(float size)		{ blur_size = size; }
		void SetCulled(bool culled)			{ this->culled = culled; }

		bool GetBlurEnabled(void)			{ return blur_enabled; }
		float GetBlurSize(void)				{ return blur_size; }
		CRenderSpace *GetRenderSpace(void)	{ return render_space; }
		bool GetCulled(void)				{ return culled; }
};



#endif
