
#include "towerengine.h"

tObject::tObject(void)
{
	rigid_body = 0;
	motion_state = 0;
}

void tTransformObject::SetTransform(tTransform transform)
{
	this->transform = transform;
	if(rigid_body)
		rigid_body->setWorldTransform(transform.ToBtTransform());
}

void tTransformObject::SetTransformWithoutRigidBody(tTransform transform)
{
	this->transform = transform;
}
