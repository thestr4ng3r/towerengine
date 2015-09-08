
#include "towerengine.h"

tGBuffer::tGBuffer(int width, int height, GLuint fbo, int first_attachment)
{
	this->fbo = fbo;
	this->first_attachment = first_attachment;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	draw_buffers = new GLenum[tex_count];
	tex_units = new int[tex_count];

	glGenTextures(tex_count, tex);

	for(int i=0; i<tex_count; i++)
	{
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		CreateTexImage((BufferType)i, width, height);
		draw_buffers[i] = GL_COLOR_ATTACHMENT0 + first_attachment + i;
		glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[i], GL_TEXTURE_2D, tex[i], 0);
		tex_units[i] = i;
	}

	printf("first gbuffer texture: %d\n", tex[0]);

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
	for(int i=0; i<tex_count; i++)
	{
		glActiveTexture(GL_TEXTURE0 + tex_units[i]);
		glBindTexture(GL_TEXTURE_2D, tex[i]);
	}
}

void tGBuffer::ChangeSize(int width, int height)
{
	for(int i=0; i<tex_count; i++)
	{
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		CreateTexImage((BufferType)i, width, height);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}


void tGBuffer::CreateTexImage(BufferType type, int width, int height)
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




