#include "towerengine.h"

void CRenderSpace::ClearObjects(void)
{
	objects.clear();
}

void CRenderSpace::Render(void)
{
	set<CObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		(*i)->PutToGL();
}
