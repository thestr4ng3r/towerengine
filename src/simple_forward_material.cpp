
#include "towerengine.h"

tSimpleForwardMaterial::tSimpleForwardMaterial(void)
{
	color = tVec(1.0, 1.0, 1.0);
	alpha = 1.0;

	tex = 0;

	blend_mode = ALPHA;
}

tSimpleForwardMaterial::~tSimpleForwardMaterial(void)
{
	if(tex != 0 && own_textures)
		glDeleteTextures(1, &tex);
}



void tSimpleForwardMaterial::SetColor(tVector color, float alpha)
{
	this->color = color;
	this->alpha = alpha;
}

void tSimpleForwardMaterial::SetBlendMode(BlendMode blend_mode)
{
	this->blend_mode = blend_mode;
}

void tSimpleForwardMaterial::LoadTexture(std::string file)
{
	if(tex != 0 && own_textures)
		glDeleteTextures(1, &tex);

	tex = LoadGLTexture(file.c_str());
}

void tSimpleForwardMaterial::LoadTexture(const char *extension, const void *data, unsigned int size)
{
	if(tex != 0 && own_textures)
		glDeleteTextures(1, &tex);

	tex = LoadGLTextureBinary(extension, data, size);
}

void tSimpleForwardMaterial::SetTexture(GLuint gl_tex)
{
	if(tex != 0 && own_textures)
		glDeleteTextures(1, &tex);

	this->tex = gl_tex;
}

bool tSimpleForwardMaterial::InitForwardPass(tDeferredRenderer *renderer, float *transform)
{
	switch(blend_mode)
	{
		case ALPHA:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_TRUE);
			break;
		case ADD:
			glBlendFunc(GL_ONE, GL_ONE);
			glDepthMask(GL_FALSE);
			break;
		case MULTIPLY:
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			glDepthMask(GL_FALSE);
			break;
	}

	tSimpleForwardShader *shader = renderer->GetSimpleForwardShader();
	shader->Bind();
	shader->SetColor(color, alpha);
	shader->SetTexture(tex != 0 ? true : false, tex);
	shader->SetTransformation(transform);

	glEnable(GL_DEPTH_TEST);

	return true;
}
