
#include "towerengine.h"

tDirectionalLightShadow::tDirectionalLightShadow(tDirectionalLight *light, int size, int splits, bool blur_enabled, float blur_size)
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
		memcpy(tex_matrix[i], tMatrix4::identity_matrix, 16 * sizeof(float));
	}
	splits_z[splits] = 0.0;

	render_space = new tRenderSpace();

	glGenTextures(1, &tex);

	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, tex);
	//glTexImage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 4, GL_RG32F, size, size, splits, GL_FALSE);
	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 0);

	glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RG32F, size, size, splits, 0, GL_RG, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenRenderbuffers(1, &depth_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);


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

	blur_draw_buffers = new GLenum[splits];
	for(i=0; i<splits; i++)
		blur_draw_buffers[i] = GL_COLOR_ATTACHMENT0 + 1 + i;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	blur_vao = new tVAO();

	blur_vertex_vbo = new tVBO<float>(2, 4);
	static const float blur_vertices[] = {	-1.0, 1.0,
											-1.0, -1.0,
											1.0, 1.0,
											1.0, -1.0};
	memcpy(blur_vertex_vbo->GetData(), blur_vertices, sizeof(float) * 8);
	blur_vertex_vbo->AssignData();

	blur_uv_vbo = new tVBO<float>(2, 4);
	static const float blur_uv_coords[] = {	0.0, 1.0,
											0.0, 0.0,
											1.0, 1.0,
											1.0, 0.0 };
	memcpy(blur_uv_vbo->GetData(), blur_uv_coords, sizeof(float) * 8);
	blur_uv_vbo->AssignData();

	blur_vao->Bind();
	blur_vertex_vbo->SetAttribute(tDirectionalShadowBlurShader::vertex_attribute, GL_FLOAT);
	blur_uv_vbo->SetAttribute(tDirectionalShadowBlurShader::uv_coord_attribute, GL_FLOAT);
	blur_vao->UnBind();

	// TODO: Use tRenderer::RenderScreenQuad for blurring
}

void tDirectionalLightShadow::Render(tCamera *camera, tRenderer *renderer)
{
	//tWorld *world = renderer->GetWorld();

	int s;
	tVector cam_pos = camera->GetPosition();
	//CVector cam_dir = world->GetCamera()->GetDirection();
	tVector light_dir, light_to, light_up, light_right;
	tVector *cam_frustum;
	float left, right, top, bottom;
	float d;

	tMatrix4 modelview_matrix, projection_matrix;
	tMatrix4 modelview_projection_matrix;

	float cam_near = camera->GetNearClip();
	float cam_far = camera->GetFarClip();
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

	//float modelview[16], projection[16];

	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glViewport(0, 0, size, size);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	renderer->SetCurrentFaceShader(renderer->GetDirectionalShadowShader());
	renderer->BindCurrentFaceShader();
	renderer->GetDirectionalShadowShader()->Bind();
	renderer->GetDirectionalShadowShader()->SetLightDir(light_dir);
	renderer->GetDirectionalShadowShader()->SetClip(near_clip, far_clip);
	renderer->GetDirectionalShadowShader()->SetCamPos(cam_pos);

	splits_z[0] = camera->GetNearClip();

	for(s=0; s<splits; s++)
	{
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0, s);

		c_log = cam_near * pow(cam_far / cam_near, (float)(s+1) / (float)splits);
		c_uni = cam_near + (cam_far - cam_near) * ((float)(s+1) / (float)splits);
		splits_z[s+1] = c_log * 0.8 + c_uni * 0.2;

		//splits_z[s+1] = world->GetCamera()->GetNearClip() +
		//		(world->GetCamera()->GetFarClip() - world->GetCamera()->GetNearClip()) * pow(2, -(splits - (s+1)));

		cam_frustum = camera->GetRelativeFrustumCorners(splits_z[s], splits_z[s+1]);

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

		modelview_matrix.SetLookAt(cam_pos, light_to, light_up);
		projection_matrix.SetOrtho(left, right, bottom, top, near_clip, far_clip);

		modelview_projection_matrix = projection_matrix * modelview_matrix;

		renderer->GetDirectionalShadowShader()->SetModelViewProjectionMatrix(modelview_projection_matrix.GetData());

		/*glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(left, right, bottom, top, near_clip, far_clip);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(cam_pos.x, cam_pos.y, cam_pos.z, light_to.x, light_to.y, light_to.z, light_up.x, light_up.y, light_up.z);

		glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
		glGetFloatv(GL_PROJECTION_MATRIX, projection);

		tMatrix4 modelview_mat(modelview);
		tMatrix4 projection_mat(projection);
		tMatrix4 mul = projection_mat * modelview_mat;*/

		memcpy(tex_matrix[s], modelview_projection_matrix.GetData(), sizeof(float) * 16);

		//CombineMatrix4(modelview, projection, tex_matrix[s]);

		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		render_space->GeometryPass(renderer);
	}

	tShader::Unbind();

	if(!blur_enabled)
		return;

	glDisable(GL_DEPTH_TEST);

	// TODO: use tRenderer::RenderScreenQuad

	glViewport(0, 0, size, size);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	renderer->GetDirectionalShadowBlurShader()->Bind();
	renderer->GetDirectionalShadowBlurShader()->SetTexture(tex);
	renderer->GetDirectionalShadowBlurShader()->SetTextureLayers(splits, h_blur);
	renderer->GetDirectionalShadowBlurShader()->SetBlurDir(Vec(1.0, 0.0) * blur_size);

	glDrawBuffers(splits, blur_draw_buffers);

	for(s=0; s<splits; s++)
		glFramebufferTextureLayer(GL_FRAMEBUFFER, blur_draw_buffers[s], blur_tex, 0, s);


	blur_vao->Draw(GL_TRIANGLE_STRIP, 0, 4);

	renderer->GetDirectionalShadowBlurShader()->SetTexture(blur_tex);
	renderer->GetDirectionalShadowBlurShader()->SetTextureLayers(splits, v_blur);
	renderer->GetDirectionalShadowBlurShader()->SetBlurDir(Vec(0.0, 1.0) * blur_size);

	for(s=0; s<splits; s++)
		glFramebufferTextureLayer(GL_FRAMEBUFFER, blur_draw_buffers[s], tex, 0, s);

	blur_vao->Draw(GL_TRIANGLE_STRIP, 0, 4);
}






