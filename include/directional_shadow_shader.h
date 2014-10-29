
#ifndef _DIRECTIONAL_SHADOW_SHADER_H
#define _DIRECTIONAL_SHADOW_SHADER_H

class tDirectionalShadowShader : public tFaceShader
{
	private:
		GLint light_dir_uniform;
		GLint clip_uniform;
		GLint cam_pos_uniform;

		GLint transformation_uniform;

		GLint vertex_mix_uniform;

		GLint diffuse_tex_enabled_uniform;
		GLint diffuse_tex_uniform;

	public:
		void Init(void);
		void SetLightDir(tVector v);
		void SetClip(float near, float far);
		void SetTransformation(const float m[16]);
		void SetVertexMix(float m = 0.0);
		void SetDiffuseTexture(bool enabled, GLuint tex = 0);
		void SetCamPos(tVector v);
};

class tDirectionalShadowBlurShader : public tShader
{
	private:
		GLint tex_uniform;
		GLint tex_layers_count_uniform;
		GLint blur_factors_uniform;
		GLint blur_dir_uniform;

		GLint blur_mode_uniform;

	public:
		static const GLint vertex_attribute = 0;
		static const GLint uv_coord_attribute = 1;

		void Init(void);
		void SetTexture(GLuint tex);
		void SetTextureLayers(int layers, float *blur_factors);
		void SetBlurDir(tVector2 v);
};

#endif
