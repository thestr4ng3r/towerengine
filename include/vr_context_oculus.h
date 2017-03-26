
#ifndef _VR_CONTEXT_OCULUS_H
#define _VR_CONTEXT_OCULUS_H

#include "vr_context.h"

#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>
#include <Extras/OVR_Math.h>

class tVRContextOculus : public tVRContext
{
	private:
		ovrSession session;
		ovrGraphicsLuid ovr_luid;
		ovrHmdDesc hmd_desc;

		OVR::Sizei tex_size[2];
		OVR::Sizei buffer_size;

		ovrTextureSwapChain swap_chain;
		ovrLayerEyeFov layer;

		int tex_count;
		std::vector<GLuint> tex;
		GLuint fbo;

		unsigned int mirror_width, mirror_height;
		ovrMirrorTexture mirror_texture;
		GLuint mirror_gl_tex;
		GLuint mirror_fbo;

		ovrEyeRenderDesc eye_render_desc[2];


		ovrTrackingState tracking_state;
		ovrPosef eye_render_pose[2];

	public:
		tVRContextOculus(float resolution_scale);
		~tVRContextOculus(void);

		unsigned int GetEyeRenderWidth(Eye eye)		{ return tex_size[eye].w; }
		unsigned int GetEyeRenderHeight(Eye eye)	{ return tex_size[eye].h; }

		GLuint GetFBO(void) { return fbo; }

		void StartFrame(tVector2 cam_rot, tVector src_pos, tVector &center_pos, tVector &center_dir, tCamera *camera[2]);
		void FinishFrame(void);

		void BlitMirrorFrame(GLint dst_x0, GLint dst_y0, GLint dst_x1, GLint dst_y1);

		void Recenter(void);


		void InitMirror(unsigned int width, unsigned int height);

		ovrSession GetOVRSession(void) { return session; }
};

#endif
