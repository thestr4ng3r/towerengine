
#ifndef _VR_CONTEXT_TEST_H
#define _VR_CONTEXT_TEST_H

class tVRContextTest : public tVRContext
{
	private:
		unsigned int eye_render_width, eye_render_height;

		GLuint render_tex;
		GLuint fbo;

	public:
		tVRContextTest(unsigned int eye_render_width, unsigned int eye_render_height);
		~tVRContextTest(void);


		unsigned int GetEyeRenderWidth(Eye eye)		{ return eye_render_width; }
		unsigned int GetEyeRenderHeight(Eye eye)	{ return eye_render_height; }

		virtual GLuint GetFBO(void)				{ return fbo; }

		void StartFrame(tVector2 cam_rot, tVector src_pos, tVector &center_pos, tVector &center_dir, tCamera *camera[2]);
		void FinishFrame(void);

		void BlitMirrorFrame(GLint dst_x0, GLint dst_y0, GLint dst_x1, GLint dst_y1);
};

#endif
