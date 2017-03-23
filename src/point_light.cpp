
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

tPointLight::tPointLight()
{
	this->pos = tVec(0.0, 0.0, 0.0);
	this->color = tVec(0.0, 0.0, 0.0);
	this->distance = 0.0;
	shadow_enabled = false;
	shadow = 0;
	enabled = true;
	shadow_invalid = false;
}

tPointLight::~tPointLight(void)
{
	delete shadow;
}

void tPointLight::InitShadow(int size, bool blur_enabled)
{
	delete shadow;

	shadow = new tPointLightShadow(this, size, blur_enabled);
	shadow_enabled = true;
	shadow_invalid = true;
}

void tPointLight::RenderShadow(tDeferredRenderer *renderer)
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

bool tPointLight::TestPointCulling(tVector point)
{
	return (point - pos).SquaredLen() > (distance * distance);
}

bool tPointLight::TestSphereCulling(tVector center, float radius)
{
	return (center - pos).SquaredLen() > (distance * distance + radius * radius);
}

bool tPointLight::TestBoundingBoxCulling(tBoundingBox b)
{
	tVector minv = b.GetMin();
	tVector maxv = b.GetMax();

	if(minv.x > pos.x + distance)
		return true;
	if(minv.y > pos.y + distance)
		return true;
	if(minv.z > pos.z + distance)
		return true;
	if(maxv.x < pos.x - distance)
		return true;
	if(maxv.y < pos.y - distance)
		return true;
	if(maxv.z < pos.z - distance)
		return true;

	return false;
}
