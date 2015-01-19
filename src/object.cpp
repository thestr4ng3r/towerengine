
#include "towerengine.h"

tObject::tObject(void)
{
}

void tTransformObject::SetTransform(tTransform transform)
{
	this->transform = transform;
	TransformChanged();
}
