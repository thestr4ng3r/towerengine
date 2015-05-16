#include "towerengine.h"

tMaterial::tMaterial(void)
{
	diffuse.enabled = false;
	diffuse.color.Set(1.0, 1.0, 1.0);
    diffuse.tex = 0;
    transparent = false;

	specular.enabled = false;
	specular.color.Set(1.0, 1.0, 1.0);
	specular.exponent = 64.0;
	specular.tex = 0;

	normal.enabled = false;
	normal.tex = 0;

	bump.enabled = false;
	bump.tex = 0;
	bump.depth = 0;

	cube_map_reflection.enabled = false;
	cube_map_reflection.color = Vec(0.0, 0.0, 0.0);
}

tMaterial::~tMaterial(void)
{
	if(diffuse.tex != 0)
		glDeleteTextures(1, &diffuse.tex);
	if(specular.tex != 0)
		glDeleteTextures(1, &specular.tex);
	if(normal.tex != 0)
		glDeleteTextures(1, &normal.tex);
}

void tMaterial::SetDiffuse(tVector color)
{
	diffuse.color = color;
}

void tMaterial::SetSpecular(tVector color, float exponent)
{
	specular.color = color;
	specular.exponent = exponent;
}

void tMaterial::SetBump(float depth)
{
	bump.depth = depth;
}

void tMaterial::SetCubeMapReflection(bool enabled, tVector color)
{
	cube_map_reflection.enabled = enabled;
	cube_map_reflection.color = color;
}

void tMaterial::LoadDiffuseTextureURL(string file)
{
	struct stat s;
	if(stat(file.c_str(), &s) == 0)
	{
		diffuse.enabled = true;
		diffuse.tex = LoadGLTexture(file.c_str(), &transparent, 3);
	}
}

void tMaterial::LoadSpecularTextureURL(string file)
{
	struct stat s;
	if(stat(file.c_str(), &s) == 0)
	{
		specular.enabled = true;
		specular.tex = LoadGLTexture(file.c_str());
	}
}

void tMaterial::LoadNormalTextureURL(string file)
{
	struct stat s;
	if(stat(file.c_str(), &s) == 0)
	{
		normal.enabled = true;
		normal.tex = LoadGLTexture(file.c_str());
	}
}

void tMaterial::LoadBumpTextureURL(string file)
{
	struct stat s;
	if(stat(file.c_str(), &s) == 0)
	{
		bump.enabled = true;
		bump.tex = LoadGLTexture(file.c_str());
	}
}

void tMaterial::LoadDiffuseTextureBinary(const char *extension, const void *data, unsigned int size)
{
	diffuse.tex = LoadGLTextureBinary(extension, data, size, &transparent, 3);
	diffuse.enabled = diffuse.tex ? 1 : 0;
}

void tMaterial::LoadSpecularTextureBinary(const char *extension, const void *data, unsigned int size)
{
	specular.tex = LoadGLTextureBinary(extension, data, size, &transparent, 3);
	specular.enabled = specular.tex ? 1 : 0;
}

void tMaterial::LoadNormalTextureBinary(const char *extension, const void *data, unsigned int size)
{
	normal.tex = LoadGLTextureBinary(extension, data, size, &transparent, 3);
	normal.enabled = normal.tex ? 1 : 0;
}

void tMaterial::LoadBumpTextureBinary(const char *extension, const void *data, unsigned int size)
{
	bump.tex = LoadGLTextureBinary(extension, data, size, &transparent, 3);
	bump.enabled = bump.tex ? 1 : 0;
}

void tMaterial::InitGeometryPass(tRenderer *renderer)
{
	renderer->GetCurrentFaceShader()->SetDiffuseTexture(diffuse.enabled, diffuse.tex);
	renderer->GetCurrentFaceShader()->SetSpecularTexture(specular.enabled, specular.tex);
	renderer->GetCurrentFaceShader()->SetNormalTexture(normal.enabled, normal.tex);
	renderer->GetCurrentFaceShader()->SetBumpTexture(bump.enabled, bump.tex);
	renderer->GetCurrentFaceShader()->SetDiffuseColor(diffuse.color);
	renderer->GetCurrentFaceShader()->SetSpecularColor(specular.color);
	renderer->GetCurrentFaceShader()->SetSpecular(specular.exponent);
	renderer->GetCurrentFaceShader()->SetBumpDepth(bump.depth);
}

void tMaterial::InitCubeMapReflectionPass(tRenderer *renderer)
{
	renderer->GetCubeMapReflectionShader()->SetReflectionColor(cube_map_reflection.color);
}
