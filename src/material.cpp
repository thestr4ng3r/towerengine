#include "towerengine.h"

using namespace std;




tDefaultMaterial::tDefaultMaterial(void)
{
	diffuse.color.Set(1.0, 1.0, 1.0);
    transparent = false;

	specular.color.Set(1.0, 1.0, 1.0);
	specular.exponent = 64.0;

	bump.enabled = false;
	bump.depth = 0;

	self_illum.color.Set(0.0, 0.0, 0.0);

	cube_map_reflection.enabled = false;
	cube_map_reflection.color = Vec(0.0, 0.0, 0.0);

	shadow.cast = true;

	for(int i=0; i<tex_count; i++)
		tex[i] = 0;

	uniform_buffer = new tUniformBuffer(20 * 4);
}

tDefaultMaterial::~tDefaultMaterial(void)
{
	for(int i=0; i<tex_count; i++)
	{
		if(tex[i] != 0)
			glDeleteTextures(1, &tex[i]);
	}

	delete uniform_buffer;
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

void tDefaultMaterial::SetShadowCast(bool enabled)
{
	shadow.cast = enabled;
}

void tDefaultMaterial::LoadTexture(TextureType type, string file)
{
	if(tex[type] != 0)
	{
		glDeleteTextures(1, &tex[type]);
	}

	bool *transparent = 0;
	if(type == DIFFUSE)
		transparent = &(this->transparent);

	int channels = 3;
	if(type == DIFFUSE)
		channels = 4;
	else if(type == BUMP)
		channels = 1;

	tex[type] = LoadGLTexture(file.c_str(), channels, transparent);
}

void tDefaultMaterial::LoadTexture(TextureType type, const char *extension, const void *data, unsigned int size)
{
	if(tex[type] != 0)
	{
		glDeleteTextures(1, &tex[type]);
	}

	bool *transparent = 0;
	if(type == DIFFUSE)
		transparent = &(this->transparent);

	int channels = 3;
	if(type == DIFFUSE)
		channels = 4;
	else if(type == BUMP)
		channels = 1;

	tex[type] = LoadGLTextureBinary(extension, data, size, channels, transparent);
}


bool tDefaultMaterial::InitDepthPrePass(tRenderer *renderer)
{
	//renderer->GetCurrentFaceShader()->SetDiffuseTexture(transparent && tex[DIFFUSE] != 0, tex[DIFFUSE]);
	renderer->GetDepthPassShader()->SetDiffuseTexture(transparent && tex[DIFFUSE] != 0, tex[DIFFUSE]);
	return true;
}


/*layout(std140) uniform MaterialBlock
{
	uniform vec3 diffuse_color;
	uniform float specular_size;

	uniform vec3 specular_color;
	uniform float bump_depth;

	uniform vec3 self_illumination_color;
	uniform bool diffuse_tex_enabled;

	uniform vec3 cube_map_reflection_color;
	uniform bool specular_tex_enabled;

	uniform bool normal_tex_enabled;
	uniform bool bump_tex_enabled;
	uniform bool self_illumination_tex_enabled;
	uniform bool cube_map_reflection_enabled;
} material_uni;*/


void tDefaultMaterial::UpdateUniformBuffer(void)
{
	float *data = (float *)uniform_buffer->GetData();

	memcpy(data, diffuse.color.v, 3 * sizeof(float));
	data[3] = specular.exponent;

	data += 4;
	memcpy(data, specular.color.v, 3 * sizeof(float));
	data[3] = bump.depth;

	data += 4;
	memcpy(data, self_illum.color.v, 3 * sizeof(float));
	*((int *)(data+3)) = tex[DIFFUSE] != 0 ? 1 : 0;

	data += 4;
	memcpy(data, cube_map_reflection.color.v, 3 * sizeof(float));
	*((int *)(data+3)) = tex[SPECULAR] != 0 ? 1 : 0;

	data += 4;
	*((int *)(data+0)) = tex[NORMAL] != 0 ? 1 : 0;
	*((int *)(data+1)) = tex[BUMP] != 0 ? 1 : 0;
	*((int *)(data+2)) = tex[SELF_ILLUMINATION] != 0 ? 1 : 0;
	*((int *)(data+3)) = cube_map_reflection.enabled != 0 ? 1 : 0;

	uniform_buffer->UploadData();
}


bool tDefaultMaterial::InitGeometryPass(tRenderer *renderer)
{
	if(renderer->GetShadowPass() && !shadow.cast)
		return false;

	uniform_buffer->Bind(tShader::material_binding_point);

	if(tex[DIFFUSE])
		renderer->GetCurrentFaceShader()->SetDiffuseTexture(tex[DIFFUSE]);

	if(tex[SPECULAR])
		renderer->GetCurrentFaceShader()->SetSpecularTexture(tex[SPECULAR]);

	if(tex[NORMAL])
		renderer->GetCurrentFaceShader()->SetNormalTexture(tex[NORMAL]);

	if(tex[BUMP])
		renderer->GetCurrentFaceShader()->SetBumpTexture(tex[BUMP]);

	if(tex[SELF_ILLUMINATION])
		renderer->GetCurrentFaceShader()->SetSelfIlluminationTexture(tex[SELF_ILLUMINATION]);

	return true;
}
