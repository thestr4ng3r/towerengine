#include "towerengine.h"

using namespace std;

tDefaultMaterial::tDefaultMaterial(void)
{
	diffuse.color.Set(1.0, 1.0, 1.0);
    //transparent = false;

	specular.color.Set(1.0, 1.0, 1.0);
	specular.exponent = 64.0;

	bump.enabled = false;
	bump.depth = 0;

	self_illum.color.Set(0.0, 0.0, 0.0);

	cube_map_reflection.enabled = false;
	cube_map_reflection.color = Vec(0.0, 0.0, 0.0);

	for(int i=0; i<tex_count; i++)
	{
		tex[i] = 0;
		tex_handle[i] = 0;
	}

	tex_handles_resident = false;
}

tDefaultMaterial::~tDefaultMaterial(void)
{
	MakeTextureHandlesResident(false);

	for(int i=0; i<tex_count; i++)
	{
		if(tex[i] != 0)
			glDeleteTextures(1, &tex[i]);
	}
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

void tDefaultMaterial::LoadTexture(TextureType type, string file)
{
	if(tex[type] != 0)
	{
		glDeleteTextures(1, &tex[type]);
		if(tex_handles_resident)
			glMakeTextureHandleNonResidentARB(tex_handle[type]);
	}

	tex[type] = LoadGLTexture(file.c_str());
	tex_handle[type] = glGetTextureHandleARB(tex[type]);

	if(tex_handles_resident)
		glMakeTextureHandleResidentARB(tex_handle[type]);
}

void tDefaultMaterial::LoadTexture(TextureType type, const char *extension, const void *data, unsigned int size)
{
	if(tex[type] != 0)
	{
		glDeleteTextures(1, &tex[type]);
		if(tex_handles_resident)
			glMakeTextureHandleNonResidentARB(tex_handle[type]);
	}

	tex[type] = LoadGLTextureBinary(extension, data, size);
	tex_handle[type] = glGetTextureHandleARB(tex[type]);

	if(tex_handles_resident)
		glMakeTextureHandleResidentARB(tex_handle[type]);
}


void tDefaultMaterial::MakeTextureHandlesResident(bool resident)
{
	if(tex_handles_resident == resident)
		return;

	for(int i=0; i<tex_count; i++)
	{
		if(tex_handle[i] == 0)
			continue;

		if(resident)
			glMakeTextureHandleResidentARB(tex_handle[i]);
		else
			glMakeTextureHandleNonResidentARB(tex_handle[i]);
	}

	tex_handles_resident = resident;
}

bool tDefaultMaterial::InitGeometryPass(tRenderer *renderer)
{
	int index = renderer->InitDefaultMaterialRender(this);
	renderer->GetCurrentFaceShader()->SetMaterialIndex(index);

	/*renderer->GetCurrentFaceShader()->SetDiffuseTexture(diffuse.tex != 0, diffuse.tex);
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
		renderer->GetCurrentFaceShader()->SetCubeMapReflectionColor(cube_map_reflection.color);*/

	return true;
}
