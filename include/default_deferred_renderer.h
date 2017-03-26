
#ifndef _DEFAULT_RENDERER_H
#define _DEFAULT_RENDERER_H

class tDefaultDeferredRenderer : public tDeferredRenderer//, public tDefaultRenderer
{
	private:
		tCamera *camera;
		tRenderSpace *camera_render_space;


	public:
		tDefaultDeferredRenderer(int width, int height, tWorld *world);
		~tDefaultDeferredRenderer(void);
		
		void Render(GLuint dst_fbo = 0, int viewport_x = 0, int viewport_y = 0, int viewport_width = 0, int viewport_height = 0);

		void ChangeSize(int width, int height)	{ ChangeScreenSize(width, height); }

		tCamera *GetCamera(void)			{ return camera; }
};

#endif