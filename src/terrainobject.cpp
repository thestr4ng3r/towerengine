#include "towerengine.h"

CTerrainObject::CTerrainObject(CTerrain *terrain)
{
	this->terrain = terrain;
}

void CTerrainObject::PutToGL(CVector cam)
{
	terrain->Paint();
}
