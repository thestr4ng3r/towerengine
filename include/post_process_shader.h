
#ifndef _POST_PROCESS_SHADER_H
#define _POST_PROCESS_SHADER_H


class tPostProcessShader : public tShader
{
	private:
		GLint color_tex_uniform;
		GLint tex_pixel_uniform;

		GLint fxaa_enabled_uniform;


	public:
		void Init(void);

		void SetTextures(GLuint color, int width, int height);
		void SetFXAA(bool enabled);
};


#endif
