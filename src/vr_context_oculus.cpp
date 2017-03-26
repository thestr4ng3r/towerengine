
#include "towerengine.h"
#include "vr_context_oculus.h"

using namespace std;
using namespace OVR;

tVRContextOculus::tVRContextOculus(float resolution_scale)
{
	if(ovr_Initialize(0) != ovrSuccess)
		throw std::exception("Failed to initialize oculus.");

	ovrResult result = ovr_Create(&session, &ovr_luid);

	if(result != ovrSuccess)
		throw std::exception("No HMD found.");


	hmd_desc = ovr_GetHmdDesc(session);
	printf("HMD: %s, resolution: %dx%d\n", hmd_desc.ProductName, hmd_desc.Resolution.w, hmd_desc.Resolution.h);


	for(int eye = 0; eye<2; eye++)
		tex_size[eye] = ovr_GetFovTextureSize(session, eye == 0 ? ovrEye_Left : ovrEye_Right, hmd_desc.DefaultEyeFov[eye], resolution_scale);


	buffer_size.w = tex_size[0].w + tex_size[1].w;
	buffer_size.h = max(tex_size[0].h, tex_size[1].h);


	swap_chain = 0;

	ovrTextureSwapChainDesc swap_chain_desc = {};
	swap_chain_desc.Type = ovrTexture_2D;
	swap_chain_desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	swap_chain_desc.ArraySize = 1;
	swap_chain_desc.Width = buffer_size.w;
	swap_chain_desc.Height = buffer_size.h;
	swap_chain_desc.MipLevels = 1;
	swap_chain_desc.SampleCount = 1;
	swap_chain_desc.StaticImage = ovrFalse;

	if(ovr_CreateTextureSwapChainGL(session, &swap_chain_desc, &swap_chain) != ovrSuccess)
	{
		printf("Failed to create texture set.\n");
		exit(1);
	}

	ovr_GetTextureSwapChainLength(session, swap_chain, &tex_count);
	tex.resize(tex_count);
	for(int i = 0; i<tex_count; i++)
	{
		ovr_GetTextureSwapChainBufferGL(session, swap_chain, i, &tex[i]);

		glBindTexture(GL_TEXTURE_2D, tex[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glGenFramebuffers(1, &fbo);

	// mirror init



	eye_render_desc[0] = ovr_GetRenderDesc(session, ovrEye_Left, hmd_desc.DefaultEyeFov[0]);
	eye_render_desc[1] = ovr_GetRenderDesc(session, ovrEye_Right, hmd_desc.DefaultEyeFov[1]);

	//ovr_SetEnabledCaps(oculus.session, ovrCap_LowPersistence|ovrHmdCap_DynamicPrediction);

	//ovr_ConfigureTracking(oculus.session, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0);
}


tVRContextOculus::~tVRContextOculus(void)
{
	ovr_Destroy(session);
	ovr_Shutdown();
}


void tVRContextOculus::StartFrame(tVector2 cam_rot, tVector src_pos, tVector& center_pos, tVector& center_dir, tCamera* camera[2])
{
	double predicted_display_time = ovr_GetPredictedDisplayTime(session, 0);
	tracking_state = ovr_GetTrackingState(session, predicted_display_time, ovrTrue);

	ovrVector3f view_offset[2] = { eye_render_desc[0].HmdToEyeOffset, eye_render_desc[1].HmdToEyeOffset };
	ovr_CalcEyePoses(tracking_state.HeadPose.ThePose, view_offset, eye_render_pose);

	if(tracking_state.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked))
	{
		OVR::Quatf orientation_quat = tracking_state.HeadPose.ThePose.Orientation;
		ovrVector3f position_ovr = tracking_state.HeadPose.ThePose.Position;

		Matrix4f manual_rot_mat = Matrix4f::RotationY(-cam_rot.x);

		position_ovr = manual_rot_mat.Transform(position_ovr);

		tVector position = Vec(position_ovr.x, position_ovr.y, position_ovr.z);
		center_pos = src_pos + position;

		Matrix4f orientation_mat = Matrix4f(orientation_quat);
		orientation_mat = manual_rot_mat * orientation_mat;

		Vector3f orientation_dir = orientation_mat.Transform(Vector3f(0.0, 0.0, -1.0));
		center_dir.x = orientation_dir.x;
		center_dir.y = orientation_dir.y;
		center_dir.z = orientation_dir.z;



		for(int eye = 0; eye<2; eye++)
		{
			//Matrix4f rollPitchYaw = Matrix4f::RotationY(Yaw);
			Matrix4f finalRollPitchYaw = manual_rot_mat * Matrix4f(eye_render_pose[eye].Orientation);
			Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
			Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
			Vector3f shiftedEyePos = manual_rot_mat.Transform(eye_render_pose[eye].Position);

			camera[eye]->SetFOV(-hmd_desc.DefaultEyeFov[eye].LeftTan, hmd_desc.DefaultEyeFov[eye].RightTan,
								hmd_desc.DefaultEyeFov[eye].DownTan, -hmd_desc.DefaultEyeFov[eye].UpTan);
			camera[eye]->SetPosition(src_pos + Vec(shiftedEyePos.x, shiftedEyePos.y, shiftedEyePos.z));
			camera[eye]->SetDirection(Vec(finalForward.x, finalForward.y, finalForward.z));
			camera[eye]->SetUp(Vec(finalUp.x, finalUp.y, finalUp.z));
		}

		/*Vector3f orientation_up = orientation_mat.Transform(Vector3f(0.0, 1.0, 0.0));
		cam_up.x = orientation_up.x;
		cam_up.y = orientation_up.y;
		cam_up.z = orientation_up.z;*/
	}

	int current_tex_index = 0;
	ovr_GetTextureSwapChainCurrentIndex(session, swap_chain, &current_tex_index);


	//ovrGLTexture *tex = (ovrGLTexture *)&oculus.texture_set->Textures[oculus.texture_set->CurrentIndex];

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex[current_tex_index], 0);

	//glViewport(0, 0, oculus.tex_size[eye].w, oculus.tex_size[eye].h);
	//glClear(GL_COLOR_BUFFER_BIT);
}

void tVRContextOculus::FinishFrame(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0); // TODO: really necessary to remove attachment?

	ovr_CommitTextureSwapChain(session, swap_chain);



	layer.Header.Type = ovrLayerType_EyeFov;
	layer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;

	for(int eye = 0; eye < 2; eye++)
	{
		layer.ColorTexture[eye] = swap_chain;
		layer.Viewport[eye] = OVR::Recti(tex_size[eye]);
		if(eye == 1)
			layer.Viewport[eye].Pos.x += tex_size[0].w;
		layer.Fov[eye] = eye_render_desc[eye].Fov; //oculus.hmd_desc.DefaultEyeFov[eye];
		layer.RenderPose[eye] = eye_render_pose[eye];
	}

	ovrLayerHeader* layers = &layer.Header;
	ovrResult result = ovr_SubmitFrame(session, 0, 0, &layers, 1);
}

void tVRContextOculus::BlitMirrorFrame(GLint dst_x0, GLint dst_y0, GLint dst_x1, GLint dst_y1)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirror_fbo);
	glBlitFramebuffer(0, mirror_height, mirror_width, 0, dst_x0, dst_y0, dst_x1, dst_y1, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}


void tVRContextOculus::Recenter(void)
{
	ovr_RecenterTrackingOrigin(session);
}

void tVRContextOculus::InitMirror(unsigned width, unsigned height)
{
	this->mirror_width = width;
	this->mirror_height = height;

	ovrMirrorTextureDesc mirror_texture_desc;
	mirror_texture_desc.Format = OVR_FORMAT_R8G8B8A8_UNORM;
	mirror_texture_desc.Width = mirror_width;
	mirror_texture_desc.Height = mirror_height;

	ovr_CreateMirrorTextureGL(session, &mirror_texture_desc, &mirror_texture);
	ovr_GetMirrorTextureBufferGL(session, mirror_texture, &mirror_gl_tex);
	//ovr_CreateMirrorTextureGL(oculus.session, GL_RGBA, screen_width, screen_height, (ovrTexture **)&oculus.mirror_texture);
	glGenFramebuffers(1, &mirror_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, mirror_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirror_gl_tex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
