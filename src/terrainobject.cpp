#include "towerengine.h"

tTerrainObject::tTerrainObject(tTerrain *terrain)
{
	this->terrain = terrain;

	motion_state = new btDefaultMotionState(btTransform(btQuaternion(0.0, 0.0, 0.0, 1), btVector3(0.0, 0.0, 0.0)));
	rigid_body = new btRigidBody(0.0, motion_state, terrain->GetCollisionShape(), btVector3(0.0, 0.0, 0.0));
	rigid_body->setRestitution(0.0);
}

void tTerrainObject::GeometryPass(void)
{
	terrain->Paint();
}

tBoundingBox tTerrainObject::GetBoundingBox(void)
{
	return terrain->GetBoundingBox();
}
