
#include "towerengine.h"

tPointLight::tPointLight(tVector pos, tVector color, float distance)
{
	this->pos = pos;
	this->color = color;
	this->distance = distance;
	shadow_enabled = false;
	shadow = 0;
	enabled = true;
	shadow_invalid = false;
}

void tPointLight::InitShadow(int size, bool blur_enabled, float blur_size)
{
	if(shadow)
		delete shadow;

	shadow = new tPointLightShadow(this, size, blur_enabled, blur_size);
	shadow_enabled = true;
	shadow_invalid = true;
}

void tPointLight::RenderShadow(tRenderer *renderer)
{
	if(!shadow_enabled)
		return;

	if(!shadow)
		return;

	if(!enabled)
		return;

	shadow->Render(renderer);
	shadow_invalid = false;
}

void tPointLight::InitRenderLighting(tPointLightingShader *shader)
{
	GLuint shadow_map = 0;

	if(shadow_enabled)
		shadow_map = shadow->GetShadowMap();

	shader->SetPointLight(pos, color, distance, shadow_enabled ? 1 : 0, shadow_map);
}
