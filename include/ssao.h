
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

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
		GLuint64 tex_handle;
		bool tex_handle_resident;

		void MakeTextureHandleResident(bool resident);
#endif

	public:
		tSSAO(tRenderer *renderer, int kernel_size, float radius, int noise_tex_size = 4);
		~tSSAO(void);

		void ChangeScreenSize(int screen_width, int screen_height);

		void Render(float viewport_scale_x = 1.0f, float viewport_offset_x = 0.0f);

		int GetWidth(void)		{ return render_width; }
		int GetHeight(void)		{ return render_height; }

		GLuint GetSSAOTexture(void)	{ return tex; }

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
		GLuint64 GetTextureHandle(void)					{ return tex_handle; }
#endif
};


#endif


