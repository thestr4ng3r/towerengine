
#include "towerengine.h"

CWorld::CWorld(void)
{
}

CWorld::~CWorld(void)
{
}

void CWorld::AddObject(CObject *o)
{
	vector<CObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(*i == o)
			return;

	objects.push_back(o);
}

void CWorld::RemoveObject(CObject *o)
{
	vector<CObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(*i == o)
		{
			objects.erase(i);
			return;
		}
}

void CWorld::PutToGL(CVector cam_pos)
{
	vector<CObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++) // TODO: Sort objects of CWorld by z-distance before painting
	{
		(*i)->PutToGL(cam_pos);
	}
}
