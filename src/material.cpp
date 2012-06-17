#include "towerengine.h"

CMaterial::CMaterial(CMesh *mesh)
{
    this->mesh = mesh;
    ibo = 0;
    if(mesh)
    {
    	mesh->AddMaterial(this);
    	ibo = mesh->CreateIBO();
    }

    diffuse.tex = 0;
	specular.tex = 0;
	normal.tex = 0;
	aeb.tex = 0;
	height.tex = 0;
	diffuse.filename = string("$NONE");
	diffuse.color.Set(1.0, 1.0, 1.0);
	diffuse.ambient_color.Set(0.5, 0.5, 0.5);
	specular.filename = string("$NONE");
	specular.color.Set(1.0, 1.0, 1.0);
	normal.filename = string("$NONE");
	aeb.filename = string("$NONE");
	aeb.bump_factor = 0.0;
	specular.exponent = 64.0;
	height.filename = string("$NONE");
	transparent = false;
	height.factor = 0.0;

    //tex = new CTexture;
}

CMaterial::~CMaterial(void)
{
	if(diffuse.tex != 0)
		glDeleteTextures(1, &diffuse.tex);
	if(specular.tex != 0)
		glDeleteTextures(1, &specular.tex);
	if(normal.tex != 0)
		glDeleteTextures(1, &normal.tex);
	if(aeb.tex != 0)
		glDeleteTextures(1, &aeb.tex);
	if(height.tex != 0)
		glDeleteTextures(1, &height.tex);

	if(mesh)
		mesh->RemoveMaterial(this);

    //delete tex;
}

CMaterial *CMaterial::CreateMaterial(const char *diffuse, const char *specular, float exponent, const char *normal, const char *aeb,
		       float b_factor, const char *height, float h_factor, char name[100], CMesh *mesh)
{
    CMaterial *m;

    m = new CMaterial(mesh);
    m->name = cstr(name);
    m->SetValues(diffuse, specular, exponent, normal, aeb, b_factor, height, h_factor);
    m->Load(".");

    return m;
}

CMaterial *CMaterial::CreateMaterialRelative(const char *path, const char *diffuse, const char *specular, float exponent, const char *normal,
		       const char *aeb, float b_factor, const char *height, float h_factor, char name[100], CMesh *mesh)
{
    CMaterial *m;

    m = new CMaterial(mesh);
    m->name = cstr(name);
    m->SetValues(diffuse, specular, exponent, normal, aeb, b_factor, height, h_factor);
    m->Load(path);

    return m;
}


void CMaterial::SetValues(const char *diffuse, const char *specular, float exponent, const char *normal, const char *aeb,
		       float b_factor, const char *height, float h_factor)
{
	this->diffuse.filename = cstr(diffuse);
	this->specular.filename = cstr(specular);
	this->normal.filename = cstr(normal);
	this->aeb.filename = cstr(aeb);
	this->height.filename = cstr(height);
	this->aeb.bump_factor = b_factor;
	this->height.factor = h_factor;
	this->specular.exponent = exponent;
}

void CMaterial::Load(string path)
{
	string file;
	struct stat s;

	file = path + diffuse.filename;
	if(diffuse.filename.compare("$NONE") == 0 || stat(file.c_str(), &s) != 0)
		diffuse.tex = GLTextureFromColor(Vec(1.0, 1.0, 1.0));
	else
		diffuse.tex = LoadGLTexture(file.c_str());

	file = path + specular.filename;
	if(specular.filename.compare("$NONE") == 0 || stat(file.c_str(), &s) != 0)
		specular.tex = GLTextureFromColor(Vec(0.5, 0.5, 0.5));
	else
		specular.tex = LoadGLTexture(file.c_str());

	file = path + normal.filename;
	if(normal.filename.compare("$NONE") == 0 || stat(file.c_str(), &s) != 0)
		normal.tex = GLTextureFromColor(Vec(0.5, 0.5, 1.0));
	else
		normal.tex = LoadGLTexture(file.c_str());

	file = path + aeb.filename;
	if(aeb.filename.compare("$NONE") == 0 || stat(file.c_str(), &s) != 0)
		aeb.tex = GLTextureFromColor(Vec(1.0, 1.0, 1.0));
	else
		aeb.tex = LoadGLTexture(file.c_str(), &transparent, 0);

	file = path + height.filename;
	if(height.filename.compare("$NONE") == 0 || stat(file.c_str(), &s) != 0)
		height.tex = GLTextureFromColor(Vec(0.5, 0.5, 0.5));
	else
		height.tex = LoadGLTexture(file.c_str());
}

void CMaterial::PutToGL(void)
{
	CEngine::GetFaceShader()->SetTexture(diffuse.tex, aeb.tex, normal.tex, height.tex, specular.tex);
	CEngine::GetFaceShader()->SetDiffuseColor(diffuse.color);
	CEngine::GetFaceShader()->SetSpecularColor(specular.color);
	CEngine::GetFaceShader()->SetAmbientColor(diffuse.ambient_color);
	CEngine::GetFaceShader()->SetSpecular(specular.exponent);
	CEngine::GetFaceShader()->SetBumpFactor(aeb.bump_factor);
	CEngine::GetFaceShader()->SetHeightFactor(Vec(1.0, 1.0, 1.0) * height.factor);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
