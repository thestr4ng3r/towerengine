
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
