#include "towerengine.h"

tMaterial::tMaterial(void)
{
	diffuse.color.Set(1.0, 1.0, 1.0);
    diffuse.tex = 0;
    transparent = false;

	specular.color.Set(1.0, 1.0, 1.0);
	specular.exponent = 64.0;
	specular.tex = 0;

	normal.tex = 0;

	bump.enabled = false;
	bump.tex = 0;
	bump.depth = 0;

	self_illum.color.Set(0.0, 0.0, 0.0);
	self_illum.tex = 0;

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
	if(bump.tex != 0)
		glDeleteTextures(1, &bump.tex);
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

void tMaterial::SetSelfIlluminationColor(tVector color)
{
	self_illum.color = color;
}

void tMaterial::LoadDiffuseTexture(string file)
{
	diffuse.tex = LoadGLTexture(file.c_str(), &transparent, 3);
}

void tMaterial::LoadSpecularTexture(string file)
{
	specular.tex = LoadGLTexture(file.c_str());
}

void tMaterial::LoadNormalTexture(string file)
{
	normal.tex = LoadGLTexture(file.c_str());
}

void tMaterial::LoadBumpTexture(string file)
{
	bump.tex = LoadGLTexture(file.c_str());
}

void tMaterial::LoadSelfIlluminationTexture(string file)
{
	self_illum.tex = LoadGLTexture(file.c_str());
}

void tMaterial::LoadDiffuseTexture(const char *extension, const void *data, unsigned int size)
{
	diffuse.tex = LoadGLTextureBinary(extension, data, size, &transparent, 3);
}

void tMaterial::LoadSpecularTexture(const char *extension, const void *data, unsigned int size)
{
	specular.tex = LoadGLTextureBinary(extension, data, size);
}

void tMaterial::LoadNormalTexture(const char *extension, const void *data, unsigned int size)
{
	normal.tex = LoadGLTextureBinary(extension, data, size);
}

void tMaterial::LoadBumpTexture(const char *extension, const void *data, unsigned int size)
{
	bump.tex = LoadGLTextureBinary(extension, data, size);
}

void tMaterial::LoadSelfIlluminationTexture(const char *extension, const void *data, unsigned int size)
{
	self_illum.tex = LoadGLTextureBinary(extension, data, size);
}

void tMaterial::InitGeometryPass(tRenderer *renderer)
{
	renderer->GetCurrentFaceShader()->SetDiffuseTexture(diffuse.tex != 0, diffuse.tex);
	renderer->GetCurrentFaceShader()->SetSpecularTexture(specular.tex != 0, specular.tex);
	renderer->GetCurrentFaceShader()->SetNormalTexture(normal.tex != 0, normal.tex);
	renderer->GetCurrentFaceShader()->SetBumpTexture(bump.tex != 0, bump.tex);
	renderer->GetCurrentFaceShader()->SetDiffuseColor(diffuse.color);
	renderer->GetCurrentFaceShader()->SetSpecularColor(specular.color);
	renderer->GetCurrentFaceShader()->SetSpecular(specular.exponent);
	renderer->GetCurrentFaceShader()->SetBumpDepth(bump.depth);
	renderer->GetCurrentFaceShader()->SetSelfIlluminationColor(self_illum.color);
	renderer->GetCurrentFaceShader()->SetSelfIlluminationTexture(self_illum.tex != 0, self_illum.tex);
}

void tMaterial::InitCubeMapReflectionPass(tRenderer *renderer)
{
	renderer->GetCubeMapReflectionShader()->SetReflectionColor(cube_map_reflection.color);
}
