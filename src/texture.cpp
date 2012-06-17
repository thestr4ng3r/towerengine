#include "towerengine.h"

CTexture::CTexture(void)
{
	diffuse_image = 0;
	specular_image = 0;
	normal_image = 0;
	aeb_image = 0;
	height_image = 0;
	diffuse.filename = NULL;
	diffuse.color.Set(1.0, 1.0, 1.0);
	diffuse.ambient_color.Set(0.5, 0.5, 0.5);
	specular.filename = NULL;
	specular.color.Set(1.0, 1.0, 1.0);
	normal.filename = NULL;
	aeb.filename = NULL;
	aeb.bump_factor = 0.0;
	aeb.exponent_factor = 64.0;
	height.filename = NULL;
	transparent = false;
	height.factor.Set(0.0, 0.0, 0.0);
}


CTexture::~CTexture(void)
{
	if(diffuse_image != 0)
		glDeleteTextures(1, &diffuse_image);
	if(specular_image != 0)
		glDeleteTextures(1, &specular_image);
	if(normal_image != 0)
		glDeleteTextures(1, &normal_image);
	if(aeb_image != 0)
		glDeleteTextures(1, &aeb_image);
	if(height_image != 0)
		glDeleteTextures(1, &height_image);
}


void CTexture::Load(void)
{
	/* Already loaded? */
	if(diffuse_image != 0 || specular_image != 0 || normal_image != 0 || aeb_image != 0 || height_image != 0)
		return;

	/* Load diffuse image */
	if(diffuse.filename != NULL)
		diffuse_image = LoadGLTexture(diffuse.filename);
	else
		diffuse_image = GLTextureFromColor(Vec(1.0, 1.0, 1.0));

	/* Load specular image */
	if(specular.filename != NULL)
		specular_image = LoadGLTexture(specular.filename);
	else
		specular_image = GLTextureFromColor(Vec(0.5, 0.5, 0.5));

	/* Load normal image */
	if(normal.filename != NULL)
		normal_image = LoadGLTexture(normal.filename);
	else
		normal_image = GLTextureFromColor(Vec(0.5, 0.5, 1.0));
	
	/* Load AlphaExponentBump image */
	if(aeb.filename != NULL)
		aeb_image = LoadGLTexture(aeb.filename, &transparent, 0);
	else
		aeb_image = GLTextureFromColor(Vec(1.0, 1.0, 1.0));

	/* Load height image */
	if(height.filename != NULL)
		height_image = LoadGLTexture(height.filename);
	else
		height_image = GLTextureFromColor(Vec(0.5, 0.5, 0.5));
}

int shader_mode = 1;

void CTexture::PutToGL(void)
{
	Load();

	CEngine::GetFaceShader()->SetTexture(diffuse_image, aeb_image, normal_image, height_image, specular_image);
	CEngine::GetFaceShader()->SetDiffuseColor(diffuse.color);
	CEngine::GetFaceShader()->SetSpecularColor(specular.color);
	CEngine::GetFaceShader()->SetAmbientColor(diffuse.ambient_color);
	CEngine::GetFaceShader()->SetSpecular(aeb.exponent_factor);
	CEngine::GetFaceShader()->SetBumpFactor(aeb.bump_factor);
	CEngine::GetFaceShader()->SetHeightFactor(height.factor);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

GLuint CTexture::default_white = 0;
GLuint CTexture::default_normal = 0;

void CTexture::LoadDefaultTextures(void)
{
	if(default_white == 0)
		default_white = GLTextureFromColor(Vec(1.0, 1.0, 1.0));
	if(default_normal == 0)
		default_normal = GLTextureFromColor(Vec(0.5, 0.5, 1.0));
}

void CTexture::PutDefaultToGL(void)
{
	LoadDefaultTextures();

	CEngine::GetFaceShader()->SetTexture(default_white, default_white, default_normal, default_white, default_white);
	CEngine::GetFaceShader()->SetDiffuseColor(Vec(1.0, 1.0, 1.0));
	CEngine::GetFaceShader()->SetSpecularColor(Vec(0.0, 0.0, 0.0));
	CEngine::GetFaceShader()->SetAmbientColor(Vec(0.5, 0.5, 0.5));
	CEngine::GetFaceShader()->SetSpecular(16.0);
	CEngine::GetFaceShader()->SetBumpFactor(0.0);
	CEngine::GetFaceShader()->SetHeightFactor(Vec(0.0, 0.0, 0.0));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void CTexture::Copy(CTexture *t)
{
	memcpy(t, this, sizeof(CTexture));
}
