#include "towerengine.h"

tTerrainObject::tTerrainObject(tTerrain *terrain)
{
	this->terrain = terrain;

	motion_state = new btDefaultMotionState(btTransform(btQuaternion(0.0, 0.0, 0.0, 1), btVector3(0.0, 0.0, 0.0)));
	rigid_body = new btRigidBody(0.0, motion_state, terrain->GetCollisionShape(), btVector3(0.0, 0.0, 0.0));
	rigid_body->setRestitution(0.0);
}

void tTerrainObject::GeometryPass(tRenderer *renderer, bool cube_map_reflection_enabled)
{
	terrain->Paint(renderer);
}

tBoundingBox tTerrainObject::GetBoundingBox(void)
{
	return terrain->GetBoundingBox();
}


void tTerrainObject::AddedToWorld(tWorld *world)
{
	if(rigid_body)
		world->GetDynamicsWorld()->addRigidBody(rigid_body);
}

void tTerrainObject::RemovedFromWorld(tWorld *world)
{
	if(rigid_body)
		world->GetDynamicsWorld()->removeRigidBody(rigid_body);
}
