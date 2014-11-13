
#include "towerengine.h"

tDirectionalLight::tDirectionalLight(tVector dir, tVector color)
{
	SetDirection(dir);
	this->color = color;
	shadow = 0;
	shadow_enabled = false;
}

void tDirectionalLight::InitShadow(int size, int splits, bool blur_enabled, float blur_size)
{
	if(shadow)
		delete shadow;

	shadow = new tDirectionalLightShadow(this, size, splits, blur_enabled, blur_size);
	shadow_enabled = true;
}

void tDirectionalLight::RenderShadow(tWorld *world)
{
	if(!shadow_enabled)
		return;

	if(!shadow)
		return;

	shadow->Render(world);
}

void tDirectionalLight::RenderLighting(void)
{
	GLuint shadow_map = 0;
	tVector2 shadow_clip = Vec(0.0, 0.0);
	float *shadow_tex_matrix = 0;
	int shadow_splits_count = 0;
	float *shadow_splits_z = 0;

	if(shadow_enabled)
	{
		shadow_map = shadow->GetShadowMap();
		shadow_clip = Vec(shadow->GetNearClip(), shadow->GetFarClip());
		shadow_splits_count = shadow->GetSplitsCount();
		shadow_tex_matrix = new float[shadow_splits_count * 16];
		for(int i=0; i<shadow->GetSplitsCount(); i++)
			memcpy(shadow_tex_matrix + (i*16), shadow->GetTextureMatrix()[i], (size_t)(sizeof(float) * 16));
		shadow_splits_z = shadow->GetSplitsZ();
	}

	tEngine::GetDirectionalLightingShader()->SetDirectionalLight(	dir,
																	color,
																	shadow_enabled ? 1 : 0,
																	shadow_map,
																	shadow_clip,
																	shadow_tex_matrix,
																	shadow_splits_count,
																	shadow_splits_z);

	tRenderer::RenderScreenQuad();
}
