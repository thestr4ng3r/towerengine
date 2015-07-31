
#ifndef _VR_RENDERER_H
#define _VR_RENDERER_H

class tVRRenderer : public tRenderer
{
	private:
		int width[2], height[2];

		tCamera *camera[2];
		tRenderSpace *camera_render_space[2];
		
	public:
		tVRRenderer(int left_width, int left_height, int right_width, int right_height, tWorld *world);
		~tVRRenderer(void);

		void Render(GLuint dst_fbo);

		tCamera *GetLeftCamera(void)	{ return camera[0]; }
		tCamera *GetRightCamera(void)	{ return camera[1]; }
};

#endif