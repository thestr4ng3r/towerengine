#include "towerengine.h"

using namespace std;




tDefaultMaterial::tDefaultMaterial(void)
{
	base_color = Vec(1.0, 1.0, 1.0);
	metallic = 0.2;
	roughness = 0.3;
	reflectance = 0.0;

	emission = Vec(0.0, 0.0, 0.0);

    transparent = false;

	bump_depth = 0;

	roughness = 0.0;

	shadow_cast = true;

	for(int i=0; i<tex_count; i++)
		tex[i] = 0;

	uniform_buffer = new tUniformBuffer(18 * 4);
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




void tDefaultMaterial::LoadTexture(TextureType type, string file)
{
	if(tex[type] != 0)
	{
		glDeleteTextures(1, &tex[type]);
	}

	bool *transparent = 0;
	if(type == BASE_COLOR)
		transparent = &(this->transparent);

	int channels = 3;
	if(type == BASE_COLOR)
		channels = 4;
	else if(type == BUMP)
		channels = 1;

	tex[type] = LoadGLTexture(file.c_str(), channels, transparent);
}

void tDefaultMaterial::LoadTexture(TextureType type, const char *extension, const void *data, size_t size)
{
	if(tex[type] != 0)
	{
		glDeleteTextures(1, &tex[type]);
	}

	bool *transparent = 0;
	if(type == BASE_COLOR)
		transparent = &(this->transparent);

	int channels = 3;
	if(type == BASE_COLOR)
		channels = 4;
	else if(type == BUMP)
		channels = 1;

	tex[type] = LoadGLTextureBinary(extension, data, size, channels, transparent);
}

void tDefaultMaterial::SetTexture(TextureType type, GLuint gl_tex)
{
	if(tex[type] != 0)
	{
		glDeleteTextures(1, &tex[type]);
	}

	tex[type] = gl_tex;
}



bool tDefaultMaterial::InitDepthPrePass(tRenderer *renderer)
{
	//renderer->GetCurrentFaceShader()->SetDiffuseTexture(transparent && tex[DIFFUSE] != 0, tex[DIFFUSE]);
	renderer->GetDepthPassShader()->SetBaseColorTexture(transparent && tex[BASE_COLOR] != 0, tex[BASE_COLOR]);
	return true;
}


/*
layout(std140) uniform MaterialBlock
{
	uniform vec3 base_color;
	uniform float metallic;

	uniform vec3 emission;
	uniform float roughness;

	uniform float reflectance;
	uniform float bump_depth;
	uniform bool base_color_tex_enabled;
	uniform bool metallic_roughness_tex_enabled;

	uniform bool normal_tex_enabled;
	uniform bool bump_tex_enabled;
	uniform bool emission_tex_enabled;
} material_uni;
 */

void tDefaultMaterial::UpdateUniformBuffer(void)
{
	float *data = (float *)uniform_buffer->GetData();

	memcpy(data, base_color.v, 3 * sizeof(float));
	data[3] = metallic;

	data += 4;
	memcpy(data, emission.v, 3 * sizeof(float));
	data[3] = roughness;

	data += 4;
	data[0] = reflectance;
	data[1] = bump_depth;
	*((int *)(data+2)) = tex[BASE_COLOR] != 0 ? 1 : 0;
	*((int *)(data+3)) = tex[METAL_ROUGH_REFLECT] != 0 ? 1 : 0;

	data += 4;
	*((int *)(data+0)) = tex[NORMAL] != 0 ? 1 : 0;
	*((int *)(data+1)) = tex[BUMP] != 0 ? 1 : 0;
	*((int *)(data+2)) = tex[EMISSION] != 0 ? 1 : 0;

	uniform_buffer->UploadData();
}


bool tDefaultMaterial::InitGeometryPass(tRenderer *renderer)
{
	if(renderer->GetShadowPass() && !shadow_cast)
		return false;

	uniform_buffer->Bind(tShader::material_binding_point);

	if(tex[BASE_COLOR])
		renderer->GetCurrentFaceShader()->SetBaseColorTexture(transparent && tex[BASE_COLOR] != 0, tex[BASE_COLOR]);

	if(tex[METAL_ROUGH_REFLECT])
		renderer->GetCurrentFaceShader()->SetMetallicRoughnessTexture(tex[METAL_ROUGH_REFLECT]);

	if(tex[NORMAL])
		renderer->GetCurrentFaceShader()->SetNormalTexture(tex[NORMAL]);

	if(tex[BUMP])
		renderer->GetCurrentFaceShader()->SetBumpTexture(tex[BUMP]);

	if(tex[EMISSION])
		renderer->GetCurrentFaceShader()->SetEmissionTexture(tex[EMISSION]);

	return true;
}
