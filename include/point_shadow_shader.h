
#ifndef _POINT_SHADOW_SHADER_H
#define _POINT_SHADOW_SHADER_H

class CPointShadowShader : public CFaceShader
{
	private:
		GLint light_pos_uniform;
		GLint light_dist_uniform;

		GLint transformation_uniform;

		GLint vertex_mix_uniform;

	public:
		void Init(void);
		void SetLightPos(CVector v);
		void SetLightDist(float d);
		void SetTransformation(const float m[16]);
		void SetVertexMix(float m = 0.0);
};

class CPointShadowBlurShader : public CShader
{
	private:
		GLint tex_uniform;
		GLint blur_dir_uniform;

	public:
		static const GLint vertex_attribute = 0;

		void Init(void);
		void SetTexture(GLuint tex);
		void SetBlurDir(CVector v);
};

#endif
