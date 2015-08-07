#include "towerengine.h"

using namespace std;

tDefaultMaterial::tDefaultMaterial(void)
{
	diffuse.color.Set(1.0, 1.0, 1.0);
    diffuse.tex = 0;
    //transparent = false;

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

tDefaultMaterial::~tDefaultMaterial(void)
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

void tDefaultMaterial::SetDiffuse(tVector color)
{
	diffuse.color = color;
}

void tDefaultMaterial::SetSpecular(tVector color, float exponent)
{
	specular.color = color;
	specular.exponent = exponent;
}

void tDefaultMaterial::SetBump(float depth)
{
	bump.depth = depth;
}

void tDefaultMaterial::SetCubeMapReflection(bool enabled, tVector color)
{
	cube_map_reflection.enabled = enabled;
	cube_map_reflection.color = color;
}

void tDefaultMaterial::SetSelfIlluminationColor(tVector color)
{
	self_illum.color = color;
}

void tDefaultMaterial::LoadDiffuseTexture(string file)
{
	diffuse.tex = LoadGLTexture(file.c_str());//, &transparent, 3);
}

void tDefaultMaterial::LoadSpecularTexture(string file)
{
	specular.tex = LoadGLTexture(file.c_str());
}

void tDefaultMaterial::LoadNormalTexture(string file)
{
	normal.tex = LoadGLTexture(file.c_str());
}

void tDefaultMaterial::LoadBumpTexture(string file)
{
	bump.tex = LoadGLTexture(file.c_str());
}

void tDefaultMaterial::LoadSelfIlluminationTexture(string file)
{
	self_illum.tex = LoadGLTexture(file.c_str());
}

void tDefaultMaterial::LoadDiffuseTexture(const char *extension, const void *data, unsigned int size)
{
	diffuse.tex = LoadGLTextureBinary(extension, data, size);//, &transparent, 3);
}

void tDefaultMaterial::LoadSpecularTexture(const char *extension, const void *data, unsigned int size)
{
	specular.tex = LoadGLTextureBinary(extension, data, size);
}

void tDefaultMaterial::LoadNormalTexture(const char *extension, const void *data, unsigned int size)
{
	normal.tex = LoadGLTextureBinary(extension, data, size);
}

void tDefaultMaterial::LoadBumpTexture(const char *extension, const void *data, unsigned int size)
{
	bump.tex = LoadGLTextureBinary(extension, data, size);
}

void tDefaultMaterial::LoadSelfIlluminationTexture(const char *extension, const void *data, unsigned int size)
{
	self_illum.tex = LoadGLTextureBinary(extension, data, size);
}

bool tDefaultMaterial::InitGeometryPass(tRenderer *renderer)
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

	renderer->GetCurrentFaceShader()->SetCubeMapReflectionEnabled(cube_map_reflection.enabled);
	if(cube_map_reflection.enabled)
		renderer->GetCurrentFaceShader()->SetCubeMapReflectionColor(cube_map_reflection.color);

	return true;
}
