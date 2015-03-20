
#ifndef _SSAO_H
#define _SSAO_H

class tRenderer;

class tSSAO
{
	private:
		tRenderer *renderer;

		tSSAOBlurShader *blur_shader;

		int render_width, render_height;

		float *kernel;
		int kernel_size;

		GLuint noise_tex;
		int noise_tex_size;

		float radius;

		GLuint fbo;
		GLuint tex;
		GLuint blur_tex;

	public:
		tSSAO(tRenderer *renderer, int kernel_size, float radius, int noise_tex_size = 4);
		~tSSAO(void);

		void ChangeScreenSize(int screen_width, int screen_height);

		void Render(void);

		int GetWidth(void)		{ return render_width; }
		int GetHeight(void)		{ return render_height; }

		GLuint GetSSAOTexture(void)	{ return tex; }
};


#endif
