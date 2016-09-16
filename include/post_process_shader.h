
#ifndef _POST_PROCESS_SHADER_H
#define _POST_PROCESS_SHADER_H


class tPostProcessShader : public tScreenShader
{
	private:
		GLint color_tex_uniform;
		GLint tex_pixel_uniform;

		GLint fxaa_enabled_uniform;


	public:
		tPostProcessShader(void);

		void SetTextures(GLuint color, int width, int height);
		void SetFXAA(bool enabled);
};


#endif
