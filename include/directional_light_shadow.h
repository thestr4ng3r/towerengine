
#ifndef _DIRECTIONAL_LIGHT_SHADOW_H
#define _DIRECTIONAL_LIGHT_SHADOW_H

class CDirectionalLightShadow
{
	private:
		CDirectionalLight *light;

		CRenderSpace *render_space;

		GLuint tex;
		GLuint fbo;
		GLuint depth_rbo;

		int size;
		int splits;

		float *splits_z;

		float near_clip, far_clip; // TODO: Set Function, dynamic setting
		float **tex_matrix;


		bool blur_enabled;
		float blur_size;
		GLuint blur_tex;
		GLuint blur_fbo;
		GLenum *blur_draw_buffers;

		VAO *blur_vao;
		VBO<float> *blur_vertex_vbo;
		VBO<float> *blur_uv_vbo;

	public:
		CDirectionalLightShadow(CDirectionalLight *light, int size, int splits, bool blur_enabled = false, float blur_size = 0.003);

		void Render(CWorld *world);

		GLuint GetShadowMap(void)			{ return tex; }
		float **GetTextureMatrix(void)		{ return tex_matrix; }
		float GetNearClip(void)				{ return near_clip; }
		float GetFarClip(void)				{ return far_clip; }
		int GetSplitsCount(void)			{ return splits; }
		float *GetSplitsZ(void)				{ return splits_z; }
		CRenderSpace *GetRenderSpace(void)	{ return render_space; }
};



#endif
