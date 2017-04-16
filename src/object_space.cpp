#include "towerengine.h"

using namespace std;

void tObjectSpace::AddObject(tObject *object)
{
	objects.insert(object);
}

void tObjectSpace::Clear(void)
{
	objects.clear();
}

void tObjectSpace::DepthPrePass(tRenderer *renderer)
{
	for(tObject *object : objects)
		object->DepthPrePass(renderer);
}


void tObjectSpace::ShadowPass(tRenderer *renderer)
{
	for(tObject *object : objects)
		object->ShadowPass(renderer);
}

void tObjectSpace::GeometryPass(tDeferredRenderer *renderer, bool cube_map_reflection_enabled)
{
	for(tObject *object : objects)
		object->GeometryPass(renderer, cube_map_reflection_enabled);
}

void tObjectSpace::ForwardPass(tRenderer *renderer)
{
	for(tObject *object : objects)
		object->ForwardPass(renderer);
}

void tObjectSpace::RefractionPass(tDeferredRenderer *renderer)
{
	for(tObject *object : objects)
		object->RefractionPass(renderer);
}

void tObjectSpace::StandardForwardPass(tForwardRenderer *renderer)
{
	for(tObject *object : objects)
		object->StandardForwardPass(renderer);
}





void tRenderObjectSpace::AddObject(tObject *object)
{
	tObjectSpace::AddObject(object);

	if(tPointLight *point_light = dynamic_cast<tPointLight *>(object))
		point_lights.insert(point_light);

	if(tDirectionalLight *dir_light = dynamic_cast<tDirectionalLight *>(object))
		dir_lights.insert(dir_light);
}

void tRenderObjectSpace::Clear(void)
{
	tObjectSpace::Clear();

	point_lights.clear();
	dir_lights.clear();
}
