
#include "towerengine.h"

CObjectSceneObject::CObjectSceneObject(CObject *object)
{
	this->object = object;
}

CObjectSceneObject::~CObjectSceneObject(void)
{
	delete object;
}

void CObjectSceneObject::AddToWorld(CWorld *world)
{
	world->AddObject(object);
}

void CObjectSceneObject::RemoveFromWorld(CWorld *world)
{
	world->RemoveObject(object);
}
