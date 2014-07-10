
#ifndef _POINT_SHADOW_SHADER_H
#define _POINT_SHADOW_SHADER_H

class CPointShadowShader : public CShader
{
	private:
		GLint light_pos_uniform;
		GLint light_dist_uniform;

		GLint transformation_uniform;

	public:
		void Init(void);
		void SetLightPos(CVector v);
		void SetLightDist(float d);
		void SetTransformation(const float m[16]);
};

class CPointShadowBlurShader : public CShader
{
	private:
		GLint tex_uniform;
		GLint blur_dir_uniform;

	public:
		void Init(void);
		void SetTexture(GLuint tex);
		void SetBlurDir(CVector v);
};

#endif
