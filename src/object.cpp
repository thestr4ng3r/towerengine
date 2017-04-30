
#include "towerengine.h"

tObject::tObject()
{
	world = 0;
	enabled = true;
}

void tObject::AddedObjectToWorld(tWorld *world)
{
	this->world = world;
	AddedToWorld(world);
}

void tObject::RemovedObjectFromWorld(tWorld *world)
{
	this->world = 0;
	RemovedFromWorld(world);
}


void tTransformObject::SetTransform(tTransform transform)
{
	this->transform = transform;
	TransformChanged();
}
