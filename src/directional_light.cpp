
#include "towerengine.h"

CDirectionalLight::CDirectionalLight(CVector dir, CVector color)
{
	SetDirection(dir);
	this->color = color;
	shadow = 0;
	shadow_enabled = false;
}

void CDirectionalLight::InitShadow(int size, int splits, bool blur_enabled, float blur_size)
{
	if(shadow)
		delete shadow;

	shadow = new CDirectionalLightShadow(this, size, splits, blur_enabled, blur_size);
	shadow_enabled = true;
}

void CDirectionalLight::RenderShadow(CWorld *world)
{
	if(!shadow_enabled)
		return;

	if(!shadow)
		return;

	shadow->Render(world);
}
