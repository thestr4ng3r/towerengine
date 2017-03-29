
#include <towerengine.h>

tVRContextTest::tVRContextTest(unsigned int eye_render_width, unsigned int eye_render_height)
{
	this->eye_render_width = eye_render_width;
	this->eye_render_height = eye_render_height;


	//openvr.system->GetProjectionRaw(vr::Eye_Left, &openvr.fov[0].left, &openvr.fov[0].right, &openvr.fov[0].top, &openvr.fov[0].bottom);
	//openvr.system->GetProjectionRaw(vr::Eye_Right, &openvr.fov[1].left, &openvr.fov[1].right, &openvr.fov[1].top, &openvr.fov[1].bottom);

	//openvr.eye_matrix[0] = ConvertOpenVRMatrix(openvr.system->GetEyeToHeadTransform(vr::Eye_Left));
	//openvr.eye_matrix[1] = ConvertOpenVRMatrix(openvr.system->GetEyeToHeadTransform(vr::Eye_Right));


	glGenTextures(1, &render_tex);
	glBindTexture(GL_TEXTURE_2D, render_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, eye_render_width * 2, eye_render_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

tVRContextTest::~tVRContextTest(void)
{
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &render_tex);
}

void tVRContextTest::StartFrame(tVector2 cam_rot, tVector src_pos, tVector &center_pos, tVector &center_dir, tCamera **camera)
{
	center_pos = src_pos; // + Vec(data[3], data[7], data[11]);
	//center_dir = -Vec(data[2], data[6], data[10]);

	double cy = cos(cam_rot.y);
	center_dir = tVec(cos(cam_rot.x) * cy, sin(cam_rot.y), sin(cam_rot.x) * cy);

	for(int i=0; i<2; i++)
	{
		tVector pos = center_pos; // + Vec(data[3], data[7], data[11]);
		tVector dir = center_dir;
		tVector up = tVec(0.0, 1.0, 0.0);

		//camera[i]->SetFOV(openvr.fov[i].left, openvr.fov[i].right, openvr.fov[i].bottom, openvr.fov[i].top);
		camera[i]->SetFOVVerticalAngle(110.0f, (float)eye_render_width / (float)eye_render_height);
		camera[i]->SetPosition(pos);
		camera[i]->SetDirection(dir);
		camera[i]->SetUp(up);
	}
}

void tVRContextTest::FinishFrame(void)
{
}

void tVRContextTest::BlitMirrorFrame(GLint dst_x0, GLint dst_y0, GLint dst_x1, GLint dst_y1)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBlitFramebuffer(0, 0, eye_render_width * 2, eye_render_height, dst_x0, dst_y0, dst_x1, dst_y1, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
