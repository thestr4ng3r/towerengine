
#include "towerengine.h"

CPointLightShadow::CPointLightShadow(CPointLight *light, int size, bool blur_enabled, float blur_size)
{
	int i;

	this->light = light;
	this->size = size;

	this->blur_enabled = blur_enabled;
	this->blur_size = blur_size;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

	for(i=0; i<6; i++)
		glTexImage2D(CubeTex(i), 0, GL_RG32F, size, size, 0, GL_RG, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glGenRenderbuffersEXT(1, &depth_rbo);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rbo);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, size, size);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);


	glGenTextures(1, &blur_tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, blur_tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

	for(i=0; i<6; i++)
		glTexImage2D(CubeTex(i), 0, GL_RG32F, size, size, 0, GL_RG, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenFramebuffersEXT(1, &blur_fbo);
}

void CPointLightShadow::Render(CWorld *world)
{
	int s;
	CVector pos = light->GetPosition();
	CVector cam_dir, cam_to;
	CVector h_vec, v_vec;

	//UseNoShader();
	glEnable(GL_DEPTH_TEST);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glViewport(0, 0, size, size);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	//glCullFace(GL_FRONT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0, 0.001, light->GetDistance());

	CEngine::SetCurrentFaceShader(CEngine::GetPointShadowShader());
	CEngine::BindCurrentFaceShader();
	CEngine::GetPointShadowShader()->SetLightPos(pos);
	CEngine::GetPointShadowShader()->SetLightDist(light->GetDistance());

	//glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	for(s=0; s<6; s++)
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+s, tex, 0);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		cam_dir = CubeVecS(s);
		cam_to = pos + cam_dir;
		v_vec = Vec(0.0, -1.0, 0.0);
		if(s == 2)
			v_vec = Vec(0.0, 0.0, 1.0);
		else if(s == 3)
			v_vec = Vec(0.0, 0.0, -1.0);
		gluLookAt(pos.x, pos.y, pos.z, cam_to.x, cam_to.y, cam_to.z, v_vec.x, v_vec.y, v_vec.z);
		world->RenderShadow();
	}
	CShader::Unbind();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);


	if(!blur_enabled)
		return;

	glDisable(GL_DEPTH_TEST);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, blur_fbo);
	glViewport(0, 0, size, size);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0, 0.001, 5.0);

	CEngine::GetPointShadowBlurShader()->Bind();
	CEngine::GetPointShadowBlurShader()->SetTexture(tex);

	for(s=0; s<6; s++)
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+s, blur_tex, 0);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		cam_dir = CubeVecS(s);
		v_vec = Vec(0.0, -1.0, 0.0);
		if(s == 2)
			v_vec = Vec(0.0, 0.0, 1.0);
		else if(s == 3)
			v_vec = Vec(0.0, 0.0, -1.0);
		gluLookAt(0.0, 0.0, 0.0, cam_dir.x, cam_dir.y, cam_dir.z, v_vec.x, v_vec.y, v_vec.z);

		h_vec = Cross(cam_dir, v_vec);

		CEngine::GetPointShadowBlurShader()->SetBlurDir(v_vec * blur_size);

		glBegin(GL_QUADS);
		glVertex3fv((cam_dir + h_vec + v_vec).v);
		glVertex3fv((cam_dir - h_vec + v_vec).v);
		glVertex3fv((cam_dir - h_vec - v_vec).v);
		glVertex3fv((cam_dir + h_vec - v_vec).v);
		glEnd();

	}
	CShader::Unbind();

	glDisable(GL_DEPTH_TEST);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glViewport(0, 0, size, size);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0, 0.001, 5.0);

	CEngine::GetPointShadowBlurShader()->Bind();
	CEngine::GetPointShadowBlurShader()->SetTexture(blur_tex);

	for(s=0; s<6; s++)
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+s, tex, 0);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		cam_dir = CubeVecS(s);
		v_vec = Vec(0.0, -1.0, 0.0);
		if(s == 2)
			v_vec = Vec(0.0, 0.0, 1.0);
		else if(s == 3)
			v_vec = Vec(0.0, 0.0, -1.0);
		gluLookAt(0.0, 0.0, 0.0, cam_dir.x, cam_dir.y, cam_dir.z, v_vec.x, v_vec.y, v_vec.z);

		h_vec = Cross(cam_dir, v_vec);

		CEngine::GetPointShadowBlurShader()->SetBlurDir(h_vec * blur_size);

		glBegin(GL_QUADS);
		glVertex3fv((cam_dir + h_vec + v_vec).v);
		glVertex3fv((cam_dir - h_vec + v_vec).v);
		glVertex3fv((cam_dir - h_vec - v_vec).v);
		glVertex3fv((cam_dir + h_vec - v_vec).v);
		glEnd();

	}
	CShader::Unbind();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}



























