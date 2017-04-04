
#ifndef _TOWERENGINE_DEFAULT_FORWARD_RENDERER_H
#define _TOWERENGINE_DEFAULT_FORWARD_RENDERER_H


class tDefaultForwardRenderer: public tForwardRenderer//, public tDefaultRenderer
{
	private:
		tCamera *camera;
		tRenderObjectSpace *camera_render_space;

	public:
		tDefaultForwardRenderer(tWorld *world);
		~tDefaultForwardRenderer();

		void Render(GLuint dst_fbo, int viewport_x, int viewport_y, int viewport_width, int viewport_height);

		tCamera *GetCamera(void)			{ return camera; }
};

#endif
