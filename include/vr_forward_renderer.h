
#ifndef _TOWERENGINE_VR_FORWARD_RENDERER_H
#define _TOWERENGINE_VR_FORWARD_RENDERER_H

class tVRForwardRenderer: public tForwardRenderer
{
	private:
		int width[2], height[2];

		int fb_width;
		int fb_height;

		bool msaa_enabled;

		GLuint fbo;
		GLuint color_tex;
		GLuint depth_tex;

		tCamera *camera[2];
		tRenderObjectSpace *camera_render_space;

		void PrepareVRRender(void);

	public:
		tVRForwardRenderer(int left_width, int left_height, int right_width, int right_height, unsigned int msaa_samples, tWorld *world);
		~tVRForwardRenderer(void);

		void Render(GLuint dst_fbo);

		tCamera *GetLeftCamera(void)	{ return camera[0]; }
		tCamera *GetRightCamera(void)	{ return camera[1]; }
};

#endif
