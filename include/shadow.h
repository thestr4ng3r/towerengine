
#ifndef _SHADOW_H
#define _SHADOW_H


class CPointLightShadow
{
	private:
		CPointLight *light;

		GLuint tex;
		GLuint fbo;
		GLuint depth_rbo;

		bool blur_enabled;
		float blur_size;
		GLuint blur_tex;
		GLuint blur_fbo;

		int size;

	public:
		CPointLightShadow(CPointLight *light, int size, bool blur_enabled = false, float blur_size = 0.003);

		void Render(CWorld *world);

		GLuint GetShadowMap(void)			{ return tex; }

		void SetBlurEnabled(bool enabled)	{ blur_enabled = enabled; }
		void SetBlurSize(float size)		{ blur_size = size; }

		bool GetBlurEnabled(void)			{ return blur_enabled; }
		float GetBlurSize(void)				{ return blur_size; }
};

class CDirectionalLightShadow
{
	private:
		CDirectionalLight *light;

		GLuint tex;
		GLuint fbo;
		GLuint depth_rbo;

		int size;
		int splits;

		float *splits_z;

		float near_clip, far_clip; // TODO: Set Function, dynamic setting
		float **tex_matrix;

	public:
		CDirectionalLightShadow(CDirectionalLight *light, int size, int splits);

		void Render(CWorld *world);

		GLuint GetShadowMap(void)		{ return tex; }
		float **GetTextureMatrix(void)	{ return tex_matrix; }
		float GetNearClip(void)			{ return near_clip; }
		float GetFarClip(void)			{ return far_clip; }
		int GetSplitsCount(void)		{ return splits; }
		float *GetSplitsZ(void)			{ return splits_z; }
};



#endif
