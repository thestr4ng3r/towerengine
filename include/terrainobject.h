#ifndef _TERRAINOBJECT_H
#define _TERRAINOBJECT_H

class tTerrainObject : public tObject
{
	private:
		tTerrain *terrain;

		btRigidBody *rigid_body;
		btMotionState *motion_state;

	public:
		tTerrainObject(tTerrain *terrain);

		void GeometryPass(tRenderer *renderer, bool cube_map_reflection_enabled);
		tBoundingBox GetBoundingBox(void);

		void AddedToWorld(tWorld *world);
		void RemovedFromWorld(tWorld *world);

		btRigidBody *GetRigidBody(void)		{ return rigid_body; }
};

#endif
