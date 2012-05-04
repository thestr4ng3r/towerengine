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

    tex = new CTexture;

    diffuse_filename = 0;
    specular_filename = 0;
    normal_filename = 0;
}

CMaterial::~CMaterial(void)
{
	if(mesh)
		mesh->RemoveMaterial(this);

    delete tex;
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
	diffuse_filename = new char[100];
	strcpy(diffuse_filename, diffuse);
	specular_filename = new char[100];
	strcpy(specular_filename, specular);
	normal_filename = new char[100];
	strcpy(normal_filename, normal);
	aeb_filename = new char[100];
	strcpy(aeb_filename, aeb);
	height_filename = new char[100];
	strcpy(height_filename, height);
	bump_factor = b_factor;
	height_factor = h_factor;
	specular_exponent = exponent;
}

void CMaterial::Load(const char *path)
{
    struct stat s;

    char *diffusea = new char[strlen(path) + strlen(diffuse_filename) + 1];
    char *speculara = new char[strlen(path) + strlen(specular_filename) + 1];
    char *normala = new char[strlen(path) + strlen(normal_filename) + 1];
    char *aeba = new char[strlen(path) + strlen(aeb_filename) + 1];
    char *heighta = new char[strlen(path) + strlen(height_filename) + 1];

    snprintf(diffusea, strlen(path) + strlen(diffuse_filename) + 1, "%s%s", path, diffuse_filename);
    snprintf(speculara, strlen(path) + strlen(specular_filename) + 1, "%s%s", path, specular_filename);
    snprintf(normala, strlen(path) + strlen(normal_filename) + 1, "%s%s", path, normal_filename);
    snprintf(aeba, strlen(path) + strlen(aeb_filename) + 1, "%s%s", path, aeb_filename);
    snprintf(heighta, strlen(path) + strlen(height_filename) + 1, "%s%s", path, height_filename);

    if(strcmp(diffuse_filename, "$NONE") == 0 || (stat(diffusea, &s) != 0 && errno == ENOENT))
        tex->diffuse.color.Set(1.0, 1.0, 1.0);
    else
        tex->diffuse.filename = diffusea;

    if(strcmp(specular_filename, "$NONE") == 0 || (stat(speculara, &s) != 0 && errno == ENOENT))
        tex->specular.color.Set(1.0, 1.0, 1.0);
    else
        tex->specular.filename = speculara;

    tex->aeb.exponent_factor = specular_exponent;

    if(strcmp(normal_filename, "$NONE") == 0 || (stat(normala, &s) != 0 && errno == ENOENT))
        tex->normal.filename = NULL;
    else
        tex->normal.filename = normala;

    if(strcmp(aeb_filename, "$NONE") == 0 || (stat(aeba, &s) != 0 && errno == ENOENT))
        tex->aeb.filename = NULL;
    else
        tex->aeb.filename = aeba;

    if(strcmp(height_filename, "$NONE") == 0 || (stat(heighta, &s) != 0 && errno == ENOENT))
        tex->height.filename = NULL;
    else
        tex->height.filename = heighta;

    tex->aeb.bump_factor = bump_factor;
    tex->height.factor = Vec(1.0, 1.0, 1.0) * height_factor;
    tex->Load();
}
