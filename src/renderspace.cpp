#include "towerengine.h"

void tRenderSpace::ClearObjects(void)
{
	objects.clear();
}

void tRenderSpace::GeometryPass(void)
{
	set<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		(*i)->GeometryPass();
}

void tRenderSpace::ForwardPass(void)
{
	set<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		(*i)->ForwardPass();
}
