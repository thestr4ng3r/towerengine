
#include "towerengine.h"

CPointLightShadow::CPointLightShadow(CPointLight *light, int size, float near_clip)
{
	int i;

	this->light = light;
	this->size = size;
	this->near_clip = near_clip;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

	for(i=0; i<6; i++)
		glTexImage2D(CubeTex(i), 0, GL_DEPTH_COMPONENT32, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void CPointLightShadow::Render(CWorld *world)
{
	int s;
	CVector pos = light->GetPosition();
	CVector cam_dir, cam_to, cam_up;

	//UseNoShader();
	glEnable(GL_DEPTH_TEST);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glViewport(0, 0, size, size);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	//glCullFace(GL_FRONT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0, near_clip, light->GetDistance());

	//glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	for(s=0; s<6; s++)
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_X+s, tex, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		cam_dir = CubeVecS(s);
		cam_to = pos + cam_dir;
		cam_up = Vec(0.0, -1.0, 0.0);
		if(s == 2)
			cam_up = Vec(0.0, 0.0, 1.0);
		else if(s == 3)
			cam_up = Vec(0.0, 0.0, -1.0);
		gluLookAt(pos.x, pos.y, pos.z, cam_to.x, cam_to.y, cam_to.z, cam_up.x, cam_up.y, cam_up.z);
		world->RenderShadow();
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	//glCullFace(GL_BACK);
}
