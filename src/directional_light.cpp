
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
