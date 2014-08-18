#include "towerengine.h"

CMaterial::CMaterial(CMesh *mesh, string name)
{
    this->mesh = mesh;
    this->name = name;
	mesh->AddMaterial(this);
	ibo = mesh->CreateIBO();

	diffuse.enabled = false;
	diffuse.filename = string();
	diffuse.color.Set(1.0, 1.0, 1.0);
	diffuse.ambient = 1.0;
    diffuse_tex = 0;
    transparent = false;

	specular.enabled = false;
	specular.filename = string();
	specular.color.Set(1.0, 1.0, 1.0);
	specular.exponent = 64.0;
	specular_tex = 0;

	normal.enabled = false;
	normal.filename = string();
	normal_tex = 0;

	height.enabled = false;
	height.filename = string();
	height.factor = 0.0;
	height_tex = 0;
}

CMaterial::~CMaterial(void)
{
	if(diffuse_tex != 0)
		glDeleteTextures(1, &diffuse_tex);
	if(specular_tex != 0)
		glDeleteTextures(1, &specular_tex);
	if(normal_tex != 0)
		glDeleteTextures(1, &normal_tex);
	if(height_tex != 0)
		glDeleteTextures(1, &height_tex);

	if(mesh)
		mesh->RemoveMaterial(this);
}

void CMaterial::SetDiffuse(bool enabled, string filename, CVector color, float ambient)
{
	diffuse.enabled = enabled;
	diffuse.filename = filename;
	diffuse.color = color;
	diffuse.ambient = ambient;
}

void CMaterial::SetSpecular(bool enabled, string filename, CVector color, float exponent)
{
	specular.enabled = enabled;
	specular.filename = filename;
	specular.color = color;
	specular.exponent = exponent;
}

void CMaterial::SetNormal(bool enabled, string filename)
{
	normal.enabled = enabled;
	normal.filename = filename;
}

void CMaterial::SetHeight(bool enabled, string filename, float factor)
{
	height.enabled = enabled;
	height.filename = filename;
	height.factor = factor;
}

void CMaterial::Load(string path)
{
	string file;
	struct stat s;

	if(diffuse.enabled)
	{
		file = path + diffuse.filename;
		if(stat(file.c_str(), &s) == 0)
			diffuse_tex = LoadGLTexture(file.c_str(), &transparent, 3);
	}

	if(specular.enabled)
	{
		file = path + specular.filename;
		if(stat(file.c_str(), &s) == 0)
			specular_tex = LoadGLTexture(file.c_str());
	}

	if(normal.enabled)
	{
		file = path + normal.filename;
		if(stat(file.c_str(), &s) == 0)
			normal_tex = LoadGLTexture(file.c_str());
	}

	if(height.enabled)
	{
		file = path + height.filename;
		if(stat(file.c_str(), &s) == 0)
			height_tex = LoadGLTexture(file.c_str());
	}
}

void CMaterial::PutToGL(void)
{
	CEngine::GetCurrentFaceShader()->SetDiffuseTexture(diffuse.enabled, diffuse_tex);
	CEngine::GetCurrentFaceShader()->SetSpecularTexture(specular.enabled, specular_tex);
	CEngine::GetCurrentFaceShader()->SetNormalTexture(normal.enabled, normal_tex);
	//CEngine::GetCurrentFaceShader()->SetHeightTexture(height.enabled, height_tex);
	CEngine::GetCurrentFaceShader()->SetDiffuseColor(diffuse.color);
	CEngine::GetCurrentFaceShader()->SetSpecularColor(specular.color);
	CEngine::GetCurrentFaceShader()->SetSpecular(specular.exponent);
	//CEngine::GetCurrentFaceShader()->SetHeightFactor(Vec(1.0, 1.0, 1.0) * height.factor);
	CEngine::GetCurrentFaceShader()->SetAmbient(diffuse.ambient);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
