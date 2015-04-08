
#include "towerengine.h"

const GLenum tPointLightShadow::blur_draw_buffers[6] = {	GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
																GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };

tPointLightShadow::tPointLightShadow(tPointLight *light, int size, bool blur_enabled, float blur_size)
{
	int i;

	this->light = light;
	this->size = size;

	culled = false;

	render_space = new tRenderSpace();

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

	for(i=0; i<6; i++)
		glTexImage2D(CubeTex(i), 0, GL_RG32F, size, size, 0, GL_RG, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenRenderbuffers(1, &depth_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	this->blur_enabled = blur_enabled;
	this->blur_size = blur_size;

	if(!blur_enabled)
		return;

	glGenTextures(1, &blur_tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, blur_tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for(i=0; i<6; i++)
		glTexImage2D(CubeTex(i), 0, GL_RG32F, size, size, 0, GL_RG, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenFramebuffers(1, &blur_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo);
	glDrawBuffers(6, blur_draw_buffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	blur_vao = new tVAO();

	blur_vertex_vbo = new tVBO<float>(2, blur_vao, 4);
	static const float blur_vertices[] = {	-1.0, 1.0,
											-1.0, -1.0,
											1.0, -1.0,
											1.0, 1.0 };
	memcpy(blur_vertex_vbo->GetData(), blur_vertices, sizeof(float) * 8);
	blur_vertex_vbo->AssignData();

	blur_vao->Bind();
	blur_vertex_vbo->SetAttribute(tPointShadowBlurShader::vertex_attribute, GL_FLOAT);
	blur_vao->UnBind();
}

void tPointLightShadow::Render(tRenderer *renderer)
{
	int s;
	tVector pos = light->GetPosition();
	tVector cam_dir, cam_to;
	tVector v_vec;

	tWorld *world = renderer->GetWorld();

	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, size, size);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0, 0.001, light->GetDistance());

	renderer->SetCurrentFaceShader(renderer->GetPointShadowShader()); // TODO: bind once in tRenderer before rendering all pointlights
	renderer->BindCurrentFaceShader();
	renderer->GetPointShadowShader()->SetLightPos(pos);
	renderer->GetPointShadowShader()->SetLightDist(light->GetDistance());

	for(s=0; s<6; s++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+s, tex, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		render_space->GeometryPass(renderer);
	}
	tShader::Unbind(); // TODO: remove this if possible
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	if(!blur_enabled)
		return;

	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo);
	glViewport(0, 0, size, size);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0, 0.001, 5.0);

	renderer->GetPointShadowBlurShader()->Bind();
	renderer->GetPointShadowBlurShader()->SetTexture(tex);

	for(s=0; s<6; s++)
		glFramebufferTexture2D(GL_FRAMEBUFFER, blur_draw_buffers[s], GL_TEXTURE_CUBE_MAP_POSITIVE_X+s, blur_tex, 0);

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);

	renderer->GetPointShadowBlurShader()->SetBlurDir(Vec(0.0, 1.0, 0.0) * blur_size);

	blur_vao->Draw(GL_QUADS, 0, 4);


	renderer->GetPointShadowBlurShader()->SetTexture(blur_tex);

	for(s=0; s<6; s++)
		glFramebufferTexture2D(GL_FRAMEBUFFER, blur_draw_buffers[s], GL_TEXTURE_CUBE_MAP_POSITIVE_X+s, tex, 0);

	glClear(GL_COLOR_BUFFER_BIT);

	renderer->GetPointShadowBlurShader()->SetBlurDir(Vec(1.0, 0.0, 0.0) * blur_size);

	blur_vao->Draw(GL_QUADS, 0, 4);

	tShader::Unbind();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}








