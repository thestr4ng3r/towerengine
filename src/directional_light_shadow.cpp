
#include "towerengine.h"

CDirectionalLightShadow::CDirectionalLightShadow(CDirectionalLight *light, int size, int splits, bool blur_enabled, float blur_size)
{
	int i;

	this->light = light;
	this->size = size;
	this->splits = splits;

	near_clip = -100.0;
	far_clip = 100.0;

	splits_z = new float[splits+1];
	tex_matrix = new float *[splits];
	for(i=0; i<splits; i++)
	{
		splits_z[i] = 0.0;
		tex_matrix[i] = new float[16];
		memcpy(tex_matrix[i], CEngine::identity_matrix4, 16 * sizeof(float));
	}
	splits_z[splits] = 0.0;

	render_space = new CRenderSpace();

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG32F, size, size, splits, 0, GL_RG, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glGenRenderbuffersEXT(1, &depth_rbo);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rbo);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, size, size);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rbo);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);


	this->blur_enabled = blur_enabled;
	this->blur_size = blur_size;

	if(!blur_enabled)
		return;

	glGenTextures(1, &blur_tex);
	glBindTexture(GL_TEXTURE_2D_ARRAY, blur_tex);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG32F, size, size, splits, 0, GL_RG, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glGenFramebuffersEXT(1, &blur_fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, blur_fbo);

	blur_draw_buffers = new GLenum[splits];
	for(i=0; i<splits; i++)
		blur_draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;

	glDrawBuffers(splits, blur_draw_buffers);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	blur_vao = new VAO();

	blur_vertex_vbo = new VBO<float>(2, blur_vao, 4);
	static const float blur_vertices[] = {	-1.0, 1.0,
											-1.0, -1.0,
											1.0, -1.0,
											1.0, 1.0 };
	memcpy(blur_vertex_vbo->GetData(), blur_vertices, sizeof(float) * 8);
	blur_vertex_vbo->AssignData();

	blur_uv_vbo = new VBO<float>(2, blur_vao, 4);
	static const float blur_uv_coords[] = {	0.0, 1.0,
											0.0, 0.0,
											1.0, 0.0,
											1.0, 1.0 };
	memcpy(blur_uv_vbo->GetData(), blur_uv_coords, sizeof(float) * 8);
	blur_uv_vbo->AssignData();

	blur_vao->Bind();
	blur_vertex_vbo->SetAttribute(CDirectionalShadowBlurShader::vertex_attribute, GL_FLOAT);
	blur_uv_vbo->SetAttribute(CDirectionalShadowBlurShader::uv_coord_attribute, GL_FLOAT);
	blur_vao->UnBind();
}

void CDirectionalLightShadow::Render(CWorld *world)
{
	int s;
	CVector cam_pos = world->GetCamera()->GetPosition();
	//CVector cam_dir = world->GetCamera()->GetDirection();
	CVector light_dir, light_to, light_up, light_right;
	CVector *cam_frustum;
	float left, right, top, bottom;
	float d;

	float cam_near = world->GetCamera()->GetNearClip();
	float cam_far = world->GetCamera()->GetFarClip();
	float c_log, c_uni;

	float *h_blur, *v_blur;

	h_blur = new float[splits];
	v_blur = new float[splits];

	light_dir = -light->GetDirection();
	light_to = cam_pos + light_dir;
	light_right = Cross(light_dir, Vec(0.0, 1.0, 0.0));
	light_right.Normalize();
	light_up = Cross(light_right, light_dir);
	light_up.Normalize();

	float modelview[16], projection[16];

	glEnable(GL_DEPTH_TEST);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glViewport(0, 0, size, size);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	CEngine::SetCurrentFaceShader(CEngine::GetDirectionalShadowShader());
	CEngine::BindCurrentFaceShader();
	CEngine::GetDirectionalShadowShader()->SetLightDir(light_dir);
	CEngine::GetDirectionalShadowShader()->SetClip(near_clip, far_clip);
	CEngine::GetDirectionalShadowShader()->SetCamPos(cam_pos);

	splits_z[0] = world->GetCamera()->GetNearClip();

	for(s=0; s<splits; s++)
	{
		glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0, tex, 0, s);

		c_log = cam_near * pow(cam_far / cam_near, (float)(s+1) / (float)splits);
		c_uni = cam_near + (cam_far - cam_near) * ((float)(s+1) / (float)splits);
		splits_z[s+1] = c_log * 0.7 + c_uni * 0.3;

		//splits_z[s+1] = world->GetCamera()->GetNearClip() +
		//		(world->GetCamera()->GetFarClip() - world->GetCamera()->GetNearClip()) * pow(2, -(splits - (s+1)));

		cam_frustum = world->GetCamera()->GetRelativeFrustumCorners(splits_z[s], splits_z[s+1]);

		left = bottom = INFINITY;
		right = top = -INFINITY;

		for(int i=0; i<8; i++)
		{
			d = Dot(cam_frustum[i], light_up);

			if(d < bottom)
				bottom = d;
			if(d > top)
				top = d;

			d = Dot(cam_frustum[i], light_right);

			if(d < left)
				left = d;
			if(d > right)
				right = d;
		}

		//left -= 0.5;
		//right += 0.5;
		//top += 0.5;
		//bottom -= 0.5;

		h_blur[s] = 1.0 / (right - left);
		v_blur[s] = 1.0 / (top - bottom);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(left, right, bottom, top, near_clip, far_clip);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(cam_pos.x, cam_pos.y, cam_pos.z, light_to.x, light_to.y, light_to.z, light_up.x, light_up.y, light_up.z);

		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
		glGetFloatv(GL_PROJECTION_MATRIX, projection);
		CombineMatrix4(modelview, projection, tex_matrix[s]);

		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		render_space->Render();
	}

	CShader::Unbind();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	if(!blur_enabled)
		return;

	glDisable(GL_DEPTH_TEST);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, blur_fbo);
	glViewport(0, 0, size, size);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);

	CEngine::GetDirectionalShadowBlurShader()->Bind();
	CEngine::GetDirectionalShadowBlurShader()->SetTexture(tex);
	CEngine::GetDirectionalShadowBlurShader()->SetTextureLayers(splits, h_blur);
	CEngine::GetDirectionalShadowBlurShader()->SetBlurDir(Vec(1.0, 0.0) * blur_size);

	for(s=0; s<splits; s++)
		glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, blur_draw_buffers[s], blur_tex, 0, s);

	blur_vao->Draw(GL_QUADS, 0, 4);

	CEngine::GetDirectionalShadowBlurShader()->SetTexture(blur_tex);
	CEngine::GetDirectionalShadowBlurShader()->SetTextureLayers(splits, v_blur);
	CEngine::GetDirectionalShadowBlurShader()->SetBlurDir(Vec(0.0, 1.0) * blur_size);

	for(s=0; s<splits; s++)
		glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, blur_draw_buffers[s], tex, 0, s);


	blur_vao->Draw(GL_QUADS, 0, 4);

	CShader::Unbind();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}






