
#ifndef _VR_RENDERER_H
#define _VR_RENDERER_H

class tVRRenderer : public tRenderer
{
	private:
		int width[2], height[2];

		tCamera *camera[2];
		tRenderSpace *camera_render_space;
		
	public:
		tVRRenderer(int left_width, int left_height, int right_width, int right_height, tWorld *world);
		~tVRRenderer(void);

		void PrepareRender(void);
		void Render(int eye, GLuint dst_fbo, bool single_texture);

		tCamera *GetLeftCamera(void)	{ return camera[0]; }
		tCamera *GetRightCamera(void)	{ return camera[1]; }
};

#endif