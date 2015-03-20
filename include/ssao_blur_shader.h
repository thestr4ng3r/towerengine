
#ifndef _SSAO_BLUR_SHADER_H
#define _SSAO_BLUR_SHADER_H


class tSSAOBlurShader : public tShader
{
	private:
		GLint tex_uniform;

		GLint blur_vertical_uniform;

	public:
		void Init(void);

		void SetTexture(GLuint tex);
		void SetBlurDirection(bool vertical);
};



#endif
