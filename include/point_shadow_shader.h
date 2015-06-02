
#ifndef _POINT_SHADOW_SHADER_H
#define _POINT_SHADOW_SHADER_H

class tPointShadowShader : public tFaceShader
{
	private:
		GLint modelview_projection_matrix_uniform;

		GLint light_pos_uniform;
		GLint light_dist_uniform;

		GLint transformation_uniform;

		GLint vertex_mix_uniform;

	public:
		void Init(void);

		void SetModelViewProjectionMatrix(float m[16]);
		void SetLightPos(tVector v);
		void SetLightDist(float d);
		void SetTransformation(const float m[16]);
		void SetVertexMix(float m = 0.0);
};

class tPointShadowBlurShader : public tShader
{
	private:
		GLint tex_uniform;
		GLint blur_dir_uniform;

	public:
		static const GLint vertex_attribute = 0;

		void Init(void);
		void SetTexture(GLuint tex);
		void SetBlurDir(tVector v);
};

#endif
