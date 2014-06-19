
#include "towerengine.h"

CPointLight::CPointLight(CVector pos, CVector color, float distance)
{
	this->pos = pos;
	this->color = color;
	this->distance = distance;
	shadow_enabled = false;
	shadow = 0;
}

void CPointLight::InitShadow(int size)
{
	if(shadow)
		delete shadow;

	shadow = new CPointLightShadow(this, size);
	shadow_enabled = true;
}

void CPointLight::RenderShadow(CWorld *world)
{
	if(!shadow_enabled)
		return;

	if(!shadow)
		return;

	shadow->Render(world);
}



CDirectionalLight::CDirectionalLight(CVector dir, CVector color)
{
	SetDirection(dir);
	this->color = color;
}

void CDirectionalLight::RenderShadow(CWorld *world)
{
}
