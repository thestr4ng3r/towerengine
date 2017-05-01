
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

		vr::IVRSystem *GetOpenVRSystem() const		{ return system; }

		unsigned int GetOpenVRControllerStates(tVector src_pos,
									   unsigned int max_controller_count,
									   vr::TrackedDeviceIndex_t *device_indexes,
									   vr::TrackedDevicePose_t *poses,
									   vr::VRControllerState_t *controller_states,
									   tMesh **render_model_meshes);
};

inline tVector tVectorFromOpenVR(const vr::HmdVector3_t &vec)
{
	return tVec(vec.v[0], vec.v[1], vec.v[2]);
}

inline tMatrix4 tMatrix4FromOpenVR(const vr::HmdMatrix34_t &mat)
{
	float v[16] = { mat.m[0][0],	mat.m[0][1],	mat.m[0][2],	mat.m[0][3],
					mat.m[1][0],	mat.m[1][1],	mat.m[1][2],	mat.m[1][3],
					mat.m[2][0],	mat.m[2][1],	mat.m[2][2],	mat.m[2][3],
					0.0f,			0.0f,			0.0f,			1.0f };
	return tMatrix4(v);
}

inline tMatrix4 tMatrix4FromOpenVR(const vr::HmdMatrix44_t &mat)
{
	return tMatrix4((float *)mat.m);
}

tMesh *tMeshFromOpenVRRenderModel(vr::RenderModel_t *render_model, vr::RenderModel_TextureMap_t *texture);

#endif
