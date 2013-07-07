
#include "towerengine.h"

CPointLight::CPointLight(CVector pos, CVector color, float distance)
{
	this->pos = pos;
	this->color = color;
	this->distance = distance;
}

CDirectionalLight::CDirectionalLight(CVector dir, CVector color)
{
	SetDirection(dir);
	this->color = color;
}
