#include "towerengine.h"

CTerrainObject::CTerrainObject(CTerrain *terrain)
{
	this->terrain = terrain;
}

void CTerrainObject::PutToGL(void)
{
	terrain->Paint();
}

CBoundingBox CTerrainObject::GetBoundingBox(void)
{
	return terrain->GetBoundingBox();
}
