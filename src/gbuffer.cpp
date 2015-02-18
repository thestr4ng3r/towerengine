
#include "towerengine.h"

tGBuffer::tGBuffer(int width, int height, GLuint fbo, int first_attachment)
{
	this->fbo = fbo;
	this->first_attachment = first_attachment;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	draw_buffers = new GLenum[tex_count];

	glGenTextures(tex_count, tex);

	glBindTexture(GL_TEXTURE_2D, tex[POSITION_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
	draw_buffers[POSITION_TEX] = GL_COLOR_ATTACHMENT0 + first_attachment + POSITION_TEX;
	glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[POSITION_TEX], GL_TEXTURE_2D, tex[POSITION_TEX], 0);

	glBindTexture(GL_TEXTURE_2D, tex[DIFFUSE_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	draw_buffers[DIFFUSE_TEX] = GL_COLOR_ATTACHMENT0 + first_attachment + DIFFUSE_TEX;
	glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[DIFFUSE_TEX], GL_TEXTURE_2D, tex[DIFFUSE_TEX], 0);

	glBindTexture(GL_TEXTURE_2D, tex[NORMAL_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	draw_buffers[NORMAL_TEX] = GL_COLOR_ATTACHMENT0 + first_attachment + NORMAL_TEX;
	glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[NORMAL_TEX], GL_TEXTURE_2D, tex[NORMAL_TEX], 0);

	glBindTexture(GL_TEXTURE_2D, tex[FACE_NORMAL_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	draw_buffers[FACE_NORMAL_TEX] = GL_COLOR_ATTACHMENT0 + first_attachment + FACE_NORMAL_TEX;
	glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[FACE_NORMAL_TEX], GL_TEXTURE_2D, tex[FACE_NORMAL_TEX], 0);

	glBindTexture(GL_TEXTURE_2D, tex[SPECULAR_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	draw_buffers[SPECULAR_TEX] = GL_COLOR_ATTACHMENT0 + first_attachment + SPECULAR_TEX;
	glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[SPECULAR_TEX], GL_TEXTURE_2D, tex[SPECULAR_TEX], 0);

	glBindTexture(GL_TEXTURE_2D, tex[SPECULAR_EXPONENT_TEX]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, 0);
	draw_buffers[SPECULAR_EXPONENT_TEX] = GL_COLOR_ATTACHMENT0 + first_attachment + SPECULAR_EXPONENT_TEX;
	glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[SPECULAR_EXPONENT_TEX], GL_TEXTURE_2D, tex[SPECULAR_EXPONENT_TEX], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

tGBuffer::~tGBuffer(void)
{
}


void tGBuffer::Bind(void)
{
	glDrawBuffers(tex_count, draw_buffers);
}

void tGBuffer::ChangeSize(int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, tex[POSITION_TEX]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_2D, tex[DIFFUSE_TEX]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, tex[NORMAL_TEX]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, tex[FACE_NORMAL_TEX]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, tex[SPECULAR_TEX]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, tex[SPECULAR_EXPONENT_TEX]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}





