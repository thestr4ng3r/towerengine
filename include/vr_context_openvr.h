
#ifndef _VR_CONTEXT_OPENVR_H
#define _VR_CONTEXT_OPENVR_H

#include "vr_context.h"

#include <openvr.h>

#include <map>

class tVRContextOpenVR : public tVRContext
{
	private:
		float resolution_scale;

		vr::IVRSystem *system;
		vr::IVRRenderModels *render_models;

		unsigned int eye_render_width, eye_render_height;

		struct fov
		{
			float left;
			float right;
			float top;
			float bottom;
		} fov[2];

		vr::HiddenAreaMesh_t hidden_area_mesh[2];
		GLuint hidden_area_stencil_rbo;

		vr::TrackedDevicePose_t tracked_device_pose[vr::k_unMaxTrackedDeviceCount];

		tMatrix4 eye_matrix[2];
		tMatrix4 hmd_matrix;

		GLuint render_tex;
		GLuint fbo;

		std::map<std::string, tMesh *> render_model_meshes;


		void RenderHiddenAreaStencil(void);

	public:
		tVRContextOpenVR(float resolution_scale);
		~tVRContextOpenVR(void);


		unsigned int GetEyeRenderWidth(Eye eye)		{ return eye_render_width; }
		unsigned int GetEyeRenderHeight(Eye eye)	{ return eye_render_height; }

		virtual GLuint GetFBO(void)					{ return fbo; }

		void StartFrame(tVector2 cam_rot, tVector src_pos, tVector &center_pos, tVector &center_dir, tCamera *camera[2]);
		void FinishFrame(void);

		void BlitMirrorFrame(GLint dst_x0, GLint dst_y0, GLint dst_x1, GLint dst_y1);

		void GetOpenVRControllerStates(tVector src_pos, vr::TrackedDevicePose_t *poses, vr::VRControllerState_t *controller_states, tMesh **render_model_meshes);
};

tMatrix4 ConvertOpenVRMatrix(const vr::HmdMatrix34_t &mat);
tMatrix4 ConvertOpenVRMatrix(const vr::HmdMatrix44_t &mat);
tMesh *ConvertOpenVRRenderModel(vr::RenderModel_t *render_model, vr::RenderModel_TextureMap_t *texture);

#endif
