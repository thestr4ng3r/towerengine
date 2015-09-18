
#include "towerengine.h"

const GLenum tPointLightShadow::blur_draw_buffers[6] = {	GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
																GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };

tPointLightShadow::tPointLightShadow(tPointLight *light, int size, bool blur_enabled)
{
	int i;

	this->light = light;
	this->size = size;

	//culled = false;

	render_object_space = new tRenderSpace();

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

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
	tex_handle = 0;
	if(tEngine::GetARBBindlessTextureSupported())
		tex_handle = glGetTextureHandleARB(tex);
	tex_resident = false;
#endif

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	for(int i=0; i<6; i++)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, tex, 0);

	glGenRenderbuffers(1, &depth_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	this->blur_enabled = blur_enabled;
	this->blur_size = 1.0 / (float)size;

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

	/*blur_vao = new tVAO();

	blur_vertex_vbo = new tVBO<float>(2, blur_vao, 4);
	static const float blur_vertices[] = {	-1.0, 1.0,
											-1.0, -1.0,
											1.0, -1.0,
											1.0, 1.0 };
	memcpy(blur_vertex_vbo->GetData(), blur_vertices, sizeof(float) * 8);
	blur_vertex_vbo->AssignData();

	blur_vao->Bind();
	blur_vertex_vbo->SetAttribute(tPointShadowBlurShader::vertex_attribute, GL_FLOAT);
	blur_vao->UnBind();*/
}

tPointLightShadow::~tPointLightShadow(void)
{
#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
	MakeTextureHandleResident(false);
#endif
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &depth_rbo);
	glDeleteTextures(1, &tex);
}

void tPointLightShadow::Render(tRenderer *renderer)
{
	int s;
	tVector pos = light->GetPosition();
	tVector cam_dir, cam_to;
	tVector v_vec;

	tMatrix4 modelview_matrix;
	tMatrix4 projection_matrix;
	tMatrix4 modelview_projection_matrix;

	//tWorld *world = renderer->GetWorld();

	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, size, size);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0, 0.001, light->GetDistance());*/

	float near_clip = 0.01;
	float far_clip = light->GetDistance();
	float clear_color[] = { far_clip, far_clip * far_clip, 0.0, 0.0 };

	projection_matrix.SetPerspective(90.0, 1.0, near_clip, far_clip);

	renderer->SetCurrentFaceShader(renderer->GetPointShadowShader()); // TODO: bind once in tRenderer before rendering all pointlights
	renderer->BindCurrentFaceShader();
	renderer->GetPointShadowShader()->Bind();
	renderer->GetPointShadowShader()->SetLightPos(pos);
	renderer->GetPointShadowShader()->SetLightDist(light->GetDistance());

	for(s=0; s<6; s++)
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + s);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearBufferfv(GL_COLOR, 0, clear_color);

		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();
		cam_dir = CubeVecS(s);
		cam_to = pos + cam_dir;
		v_vec = Vec(0.0, -1.0, 0.0);
		if(s == 2)
			v_vec = Vec(0.0, 0.0, 1.0);
		else if(s == 3)
			v_vec = Vec(0.0, 0.0, -1.0);
		//gluLookAt(pos.x, pos.y, pos.z, cam_to.x, cam_to.y, cam_to.z, v_vec.x, v_vec.y, v_vec.z);
		modelview_matrix.SetLookAt(pos, cam_to, v_vec);

		modelview_projection_matrix = projection_matrix * modelview_matrix;
		renderer->GetPointShadowShader()->SetModelViewProjectionMatrix(modelview_projection_matrix.GetData());

		render_object_space->GeometryPass(renderer);
	}
	tShader::Unbind(); // TODO: remove this if possible
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	if(!blur_enabled)
		return;

	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo);
	glViewport(0, 0, size, size);

	renderer->GetPointShadowBlurShader()->Bind();
	renderer->GetPointShadowBlurShader()->SetTexture(tex);

	for(s=0; s<6; s++)
		glFramebufferTexture2D(GL_FRAMEBUFFER, blur_draw_buffers[s], GL_TEXTURE_CUBE_MAP_POSITIVE_X+s, blur_tex, 0);

	glClear(GL_COLOR_BUFFER_BIT);

	renderer->GetPointShadowBlurShader()->SetBlurDir(Vec(0.0, 1.0, 0.0) * blur_size);

	//blur_vao->Draw(GL_QUADS, 0, 4);
	renderer->RenderScreenQuad();


	renderer->GetPointShadowBlurShader()->SetTexture(blur_tex);

	for(s=0; s<6; s++)
		glFramebufferTexture2D(GL_FRAMEBUFFER, blur_draw_buffers[s], GL_TEXTURE_CUBE_MAP_POSITIVE_X+s, tex, 0);

	glClear(GL_COLOR_BUFFER_BIT);

	renderer->GetPointShadowBlurShader()->SetBlurDir(Vec(1.0, 0.0, 0.0) * blur_size);

	//blur_vao->Draw(GL_QUADS, 0, 4);

	renderer->RenderScreenQuad();

	tShader::Unbind();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
void tPointLightShadow::MakeTextureHandleResident(bool resident)
{
	if(tex_handle == 0 || tex_resident == resident)
		return;

	if(resident)
		glMakeTextureHandleResidentARB(tex_handle);
	else
		glMakeTextureHandleNonResidentARB(tex_handle);

	tex_resident = resident;
}
#endif







