#include "towerengine.h"

void CRenderSpace::ClearObjects(void)
{
	objects.clear();
}

void CRenderSpace::GeometryPass(void)
{
	set<CObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		(*i)->GeometryPass();
}

void CRenderSpace::ForwardPass(void)
{
	set<CObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		(*i)->ForwardPass();
}
