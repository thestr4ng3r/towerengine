
#ifndef _POST_PROCESS_SHADER_H
#define _POST_PROCESS_SHADER_H


class CPostProcessShader : public CShader
{
	private:
		GLint color_tex_uniform;
		GLint depth_tex_uniform;
		GLint tex_pixel_uniform;


	public:
		void Init(void);

		void SetTextures(GLuint color, GLuint depth, int width, int height);
};


#endif
