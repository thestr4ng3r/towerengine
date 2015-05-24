#include "towerengine.h"

using namespace std;

void tRenderObjectSpace::Clear(void)
{
	objects.clear();
}

void tRenderObjectSpace::GeometryPass(tRenderer *renderer)
{
	set<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		(*i)->GeometryPass(renderer);
}

void tRenderObjectSpace::ForwardPass(tRenderer *renderer)
{
	set<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		(*i)->ForwardPass(renderer);
}


void tRenderSpace::Clear(void)
{
	tRenderObjectSpace::Clear();

	point_lights.clear();
	dir_lights.clear();
}
