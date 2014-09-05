#include "towerengine.h"

CMaterial::CMaterial(CMesh *mesh, string name)
{
	ibo_data = 0;
	triangle_count = 0;
	triangle_i = 0;

    this->mesh = mesh;
    this->name = name;
	mesh->AddMaterial(this);
	ibo = mesh->CreateIBO();

	diffuse.enabled = false;
	diffuse.color.Set(1.0, 1.0, 1.0);
	diffuse.ambient = 1.0;
    diffuse.tex = 0;
    transparent = false;

	specular.enabled = false;
	specular.color.Set(1.0, 1.0, 1.0);
	specular.exponent = 64.0;
	specular.tex = 0;

	normal.enabled = false;
	normal.tex = 0;
}

CMaterial::~CMaterial(void)
{
	if(diffuse.tex != 0)
		glDeleteTextures(1, &diffuse.tex);
	if(specular.tex != 0)
		glDeleteTextures(1, &specular.tex);
	if(normal.tex != 0)
		glDeleteTextures(1, &normal.tex);

	if(mesh)
		mesh->RemoveMaterial(this);
}

void CMaterial::SetDiffuse(CVector color, float ambient)
{
	diffuse.color = color;
	diffuse.ambient = ambient;
}

void CMaterial::SetSpecular(CVector color, float exponent)
{
	specular.color = color;
	specular.exponent = exponent;
}


void CMaterial::LoadDiffuseTextureURL(string file)
{
	struct stat s;
	if(stat(file.c_str(), &s) == 0)
	{
		diffuse.enabled = true;
		diffuse.tex = LoadGLTexture(file.c_str(), &transparent, 3);
	}
}

void CMaterial::LoadSpecularTextureURL(string file)
{
	struct stat s;
	if(stat(file.c_str(), &s) == 0)
	{
		specular.enabled = true;
		specular.tex = LoadGLTexture(file.c_str());
	}
}

void CMaterial::LoadNormalTextureURL(string file)
{
	struct stat s;
	if(stat(file.c_str(), &s) == 0)
	{
		normal.enabled = true;
		normal.tex = LoadGLTexture(file.c_str());
	}
}

void CMaterial::LoadDiffuseTextureBinary(const char *extension, const void *data, unsigned int size)
{
	diffuse.tex = LoadGLTextureBinary(extension, data, size, &transparent, 3);
	diffuse.enabled = diffuse.tex ? 1 : 0;
}

void CMaterial::LoadSpecularTextureBinary(const char *extension, const void *data, unsigned int size)
{
	specular.tex = LoadGLTextureBinary(extension, data, size, &transparent, 3);
	specular.enabled = specular.tex ? 1 : 0;
}

void CMaterial::LoadNormalTextureBinary(const char *extension, const void *data, unsigned int size)
{
	normal.tex = LoadGLTextureBinary(extension, data, size, &transparent, 3);
	normal.enabled = normal.tex ? 1 : 0;
}

void CMaterial::PutToGL(void)
{
	CEngine::GetCurrentFaceShader()->SetDiffuseTexture(diffuse.enabled, diffuse.tex);
	CEngine::GetCurrentFaceShader()->SetSpecularTexture(specular.enabled, specular.tex);
	CEngine::GetCurrentFaceShader()->SetNormalTexture(normal.enabled, normal.tex);
	CEngine::GetCurrentFaceShader()->SetDiffuseColor(diffuse.color);
	CEngine::GetCurrentFaceShader()->SetSpecularColor(specular.color);
	CEngine::GetCurrentFaceShader()->SetSpecular(specular.exponent);
	CEngine::GetCurrentFaceShader()->SetAmbient(diffuse.ambient);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
