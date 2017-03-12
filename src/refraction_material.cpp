
#include "towerengine.h"

tRefractionMaterial::tRefractionMaterial(void)
{
	color = Vec(1.0, 1.0, 1.0);
	edge_color = Vec(1.0, 1.0, 1.0);
	edge_alpha = 0.0;
	color_tex = 0;
	normal_tex = 0;
}

tRefractionMaterial::~tRefractionMaterial(void)
{
	if(own_textures)
	{
		if(color_tex != 0)
			glDeleteTextures(1, &color_tex);

		if(normal_tex != 0)
			glDeleteTextures(1, &normal_tex);
	}
}



void tRefractionMaterial::SetColor(tVector color)
{
	this->color = color;
}

void tRefractionMaterial::SetEdgeColor(tVector edge_color, float alpha)
{
	this->edge_color = edge_color;
	this->edge_alpha = alpha;
}

void tRefractionMaterial::LoadColorTexture(std::string file)
{
	if(color_tex != 0 && own_textures)
		glDeleteTextures(1, &color_tex);

	color_tex = LoadGLTexture(file.c_str());
}

void tRefractionMaterial::LoadColorTexture(const char *extension, const void *data, unsigned int size)
{
	if(color_tex != 0 && own_textures)
		glDeleteTextures(1, &color_tex);

	color_tex = LoadGLTextureBinary(extension, data, size);
}

void tRefractionMaterial::SetColorTexture(GLuint gl_tex)
{
	if(color_tex != 0 && own_textures)
		glDeleteTextures(1, &color_tex);

	color_tex = gl_tex;
}

void tRefractionMaterial::LoadNormalTexture(std::string file)
{
	if(normal_tex != 0 && own_textures)
		glDeleteTextures(1, &normal_tex);

	normal_tex = LoadGLTexture(file.c_str());
}

void tRefractionMaterial::LoadNormalTexture(const char *extension, const void *data, unsigned int size)
{
	if(normal_tex != 0 && own_textures)
		glDeleteTextures(1, &normal_tex);

	normal_tex = LoadGLTextureBinary(extension, data, size);
}

void tRefractionMaterial::SetNormalTexture(GLuint gl_tex)
{
	if(normal_tex != 0 && own_textures)
		glDeleteTextures(1, &normal_tex);

	normal_tex = gl_tex;
}

bool tRefractionMaterial::InitRefractionPass(tRenderer *renderer, float *transform)
{
	tRefractionShader *shader = renderer->GetRefractionShader();
	shader->Bind();
	shader->SetColor(color, edge_color, edge_alpha);
	shader->SetColorTexture(color_tex != 0, color_tex);
	shader->SetNormalTexture(normal_tex != 0, normal_tex);
	shader->SetTransformation(transform);
	shader->SetCameraPosition(renderer->GetCurrentRenderingCamera()->GetPosition());
	shader->SetScreenTexture(renderer->GetCurrentReadColorTexture());

	glBlendFunc(GL_ONE, GL_ZERO);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	return true;
}
