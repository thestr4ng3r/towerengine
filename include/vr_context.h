
#ifndef _VR_CONTEXT_H
#define _VR_CONTEXT_H

class tVRContext
{
	public:
		enum Eye { LEFT = 0, RIGHT = 1 };

		virtual ~tVRContext(void) {};

		virtual unsigned int GetEyeRenderWidth(Eye eye) = 0;
		virtual unsigned int GetEyeRenderHeight(Eye eye) = 0;

		virtual GLuint GetFBO(void) = 0;
		virtual GLuint GetMirrorFBO(void) { return GetFBO(); }

		virtual void StartFrame(tVector2 cam_rot, tVector src_pos, tVector &center_pos, tVector &center_dir, tCamera *camera[2]) = 0;
		virtual void FinishFrame(void) = 0;

		virtual void BlitMirrorFrame(GLint dst_x0, GLint dst_y0, GLint dst_x1, GLint dst_y1) =0;

		virtual void Recenter(void) {}
};

#endif
