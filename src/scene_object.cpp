
#include "towerengine.h"

tObjectSceneObject::tObjectSceneObject(tObject *object)
{
	this->object = object;
}

tObjectSceneObject::~tObjectSceneObject(void)
{
	delete object;
}

void tObjectSceneObject::AddToWorld(tWorld *world)
{
	world->AddObject(object);
}

void tObjectSceneObject::RemoveFromWorld(tWorld *world)
{
	world->RemoveObject(object);
}


tDirectionalLightSceneObject::tDirectionalLightSceneObject(tDirectionalLight *light)
{
	this->light = light;
}

tDirectionalLightSceneObject::~tDirectionalLightSceneObject(void)
{
	delete light;
}

void tDirectionalLightSceneObject::AddToWorld(tWorld *world)
{
	world->AddDirectionalLight(light);
}

void tDirectionalLightSceneObject::RemoveFromWorld(tWorld *world)
{
	world->RemoveDirectionalLight(light);
}


tPointLightSceneObject::tPointLightSceneObject(tPointLight *light)
{
	this->light = light;
}

tPointLightSceneObject::~tPointLightSceneObject(void)
{
	delete light;
}

void tPointLightSceneObject::AddToWorld(tWorld *world)
{
	world->AddPointLight(light);
}

void tPointLightSceneObject::RemoveFromWorld(tWorld *world)
{
	world->RemovePointLight(light);
}
