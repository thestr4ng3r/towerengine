#include "towerengine.h"


tVertex::tVertex(void)
{
	pos = Vec(0.0, 0.0, 0.0);
	id = -1;
}

tVertex::tVertex(tVector v)
{
	pos = v;
	id = -1;
}

tVertex::~tVertex(void)
{
}
