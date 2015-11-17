#include "towerengine.h"

using namespace std;

void tRenderObjectSpace::Clear(void)
{
	objects.clear();
}

void tRenderObjectSpace::DepthPrePass(tRenderer *renderer)
{
	set<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		(*i)->DepthPrePass(renderer);
}

void tRenderObjectSpace::GeometryPass(tRenderer *renderer, bool cube_map_reflection_enabled)
{
	set<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		(*i)->GeometryPass(renderer, cube_map_reflection_enabled);
}

void tRenderObjectSpace::ForwardPass(tRenderer *renderer)
{
	set<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		(*i)->ForwardPass(renderer);
}

void tRenderObjectSpace::RefractionPass(tRenderer *renderer)
{
	set<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		(*i)->RefractionPass(renderer);
}


void tRenderSpace::Clear(void)
{
	tRenderObjectSpace::Clear();

	point_lights.clear();
	dir_lights.clear();
}
