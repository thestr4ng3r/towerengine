
#ifndef _DIRECTIONAL_LIGHT_SHADOW_H
#define _DIRECTIONAL_LIGHT_SHADOW_H

class tDirectionalLightShadow
{
	private:
		tDirectionalLight *light;

		tRenderObjectSpace *render_space;

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
		GLenum *blur_draw_buffers;

		tVAO *blur_vao;
		tVBO<float> *blur_vertex_vbo;
		tVBO<float> *blur_uv_vbo;

	public:
		tDirectionalLightShadow(tDirectionalLight *light, int size, int splits, bool blur_enabled = false, float blur_size = 0.003);

		void Render(tCamera *camera, tRenderer *renderer);

		GLuint GetShadowMap(void)			{ return tex; }
		float **GetTextureMatrix(void)		{ return tex_matrix; }
		float GetNearClip(void)				{ return near_clip; }
		float GetFarClip(void)				{ return far_clip; }
		int GetSplitsCount(void)			{ return splits; }
		float *GetSplitsZ(void)				{ return splits_z; }
		tRenderObjectSpace *GetRenderSpace(void)	{ return render_space; }

		void SetNearClip(float v)			{ near_clip = v; }
		void SetFarClip(float v)			{ far_clip = v; }
};



#endif
