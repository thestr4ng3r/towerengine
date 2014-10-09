#ifndef _TERRAINOBJECT_H
#define _TERRAINOBJECT_H

class CTerrainObject : public CObject
{
	private:
		CTerrain *terrain;

	public:
		CTerrainObject(CTerrain *terrain);

		void GeometryPass(void);
		CBoundingBox GetBoundingBox(void);
};

#endif
