#ifndef _TERRAINOBJECT_H
#define _TERRAINOBJECT_H

class tTerrainObject : public tObject
{
	private:
		tTerrain *terrain;

	public:
		tTerrainObject(tTerrain *terrain);

		void GeometryPass(void);
		tBoundingBox GetBoundingBox(void);
};

#endif
