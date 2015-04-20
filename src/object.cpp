
#include "towerengine.h"

tObject::tObject(void)
{
	//tag = "";
}

void tTransformObject::SetTransform(tTransform transform)
{
	this->transform = transform;
	TransformChanged();
}
