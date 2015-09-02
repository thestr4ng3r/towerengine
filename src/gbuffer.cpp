
#include "towerengine.h"

tGBuffer::tGBuffer(int width, int height, GLuint fbo, int first_attachment)
{
	this->fbo = fbo;
	this->first_attachment = first_attachment;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	draw_buffers = new GLenum[tex_count];
	tex_units = new int[tex_count];

	glGenTextures(tex_count, tex);

	glBindTexture(GL_TEXTURE_2D, tex[POSITION_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	CreateTexImage(POSITION_TEX, width, height);
	draw_buffers[POSITION_TEX] = GL_COLOR_ATTACHMENT0 + first_attachment + POSITION_TEX;
	glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[POSITION_TEX], GL_TEXTURE_2D, tex[POSITION_TEX], 0);
	tex_units[POSITION_TEX] = 0;

	glBindTexture(GL_TEXTURE_2D, tex[DIFFUSE_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	CreateTexImage(DIFFUSE_TEX, width, height);
	draw_buffers[DIFFUSE_TEX] = GL_COLOR_ATTACHMENT0 + first_attachment + DIFFUSE_TEX;
	glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[DIFFUSE_TEX], GL_TEXTURE_2D, tex[DIFFUSE_TEX], 0);
	tex_units[DIFFUSE_TEX] = 1;

	glBindTexture(GL_TEXTURE_2D, tex[NORMAL_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	CreateTexImage(NORMAL_TEX, width, height);
	draw_buffers[NORMAL_TEX] = GL_COLOR_ATTACHMENT0 + first_attachment + NORMAL_TEX;
	glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[NORMAL_TEX], GL_TEXTURE_2D, tex[NORMAL_TEX], 0);
	tex_units[NORMAL_TEX] = 2;

	glBindTexture(GL_TEXTURE_2D, tex[FACE_NORMAL_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	CreateTexImage(FACE_NORMAL_TEX, width, height);
	draw_buffers[FACE_NORMAL_TEX] = GL_COLOR_ATTACHMENT0 + first_attachment + FACE_NORMAL_TEX;
	glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[FACE_NORMAL_TEX], GL_TEXTURE_2D, tex[FACE_NORMAL_TEX], 0);
	tex_units[FACE_NORMAL_TEX] = 3;

	glBindTexture(GL_TEXTURE_2D, tex[SPECULAR_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	CreateTexImage(SPECULAR_TEX, width, height);
	draw_buffers[SPECULAR_TEX] = GL_COLOR_ATTACHMENT0 + first_attachment + SPECULAR_TEX;
	glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[SPECULAR_TEX], GL_TEXTURE_2D, tex[SPECULAR_TEX], 0);
	tex_units[SPECULAR_TEX] = 4;

	glBindTexture(GL_TEXTURE_2D, tex[SELF_ILLUMINATION_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	CreateTexImage(SELF_ILLUMINATION_TEX, width, height);
	draw_buffers[SELF_ILLUMINATION_TEX] = GL_COLOR_ATTACHMENT0 + first_attachment + SELF_ILLUMINATION_TEX;
	glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[SELF_ILLUMINATION_TEX], GL_TEXTURE_2D, tex[SELF_ILLUMINATION_TEX], 0);
	tex_units[SELF_ILLUMINATION_TEX] = 5;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

tGBuffer::~tGBuffer(void)
{
}


void tGBuffer::BindDrawBuffers(void)
{
	glDrawBuffers(tex_count, draw_buffers);
}

void tGBuffer::BindTextures(void)
{
	glActiveTexture(GL_TEXTURE0 + tex_units[POSITION_TEX]);
	glBindTexture(GL_TEXTURE_2D, tex[POSITION_TEX]);

	glActiveTexture(GL_TEXTURE0 + tex_units[DIFFUSE_TEX]);
	glBindTexture(GL_TEXTURE_2D, tex[DIFFUSE_TEX]);

	glActiveTexture(GL_TEXTURE0 + tex_units[NORMAL_TEX]);
	glBindTexture(GL_TEXTURE_2D, tex[NORMAL_TEX]);

	glActiveTexture(GL_TEXTURE0 + tex_units[FACE_NORMAL_TEX]);
	glBindTexture(GL_TEXTURE_2D, tex[FACE_NORMAL_TEX]);

	glActiveTexture(GL_TEXTURE0 + tex_units[SPECULAR_TEX]);
	glBindTexture(GL_TEXTURE_2D, tex[SPECULAR_TEX]);

	glActiveTexture(GL_TEXTURE0 + tex_units[SELF_ILLUMINATION_TEX]);
	glBindTexture(GL_TEXTURE_2D, tex[SELF_ILLUMINATION_TEX]);
}

void tGBuffer::ChangeSize(int width, int height)
{
	for(int i=0; i<tex_count; i++)
	{
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		CreateTexImage((GBUFFER_TEXTURE_TYPE)i, width, height);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}


void tGBuffer::CreateTexImage(GBUFFER_TEXTURE_TYPE type, int width, int height)
{
	switch(type)
	{
		case POSITION_TEX:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
			break;
		case DIFFUSE_TEX:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			break;
		case NORMAL_TEX:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
			break;
		case FACE_NORMAL_TEX:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
			break;
		case SPECULAR_TEX:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
			break;
		case SELF_ILLUMINATION_TEX:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
			break;
	}
}




