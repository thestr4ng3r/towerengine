#include "towerengine.h"

void tRenderSpace::ClearObjects(void)
{
	objects.clear();
}

void tRenderSpace::GeometryPass(tRenderer *renderer)
{
	set<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		(*i)->GeometryPass(renderer);
}

void tRenderSpace::ForwardPass(tRenderer *renderer)
{
	set<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		(*i)->ForwardPass(renderer);
}
