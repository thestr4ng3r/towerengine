#include "towerengine.h"

tTerrainObject::tTerrainObject(tTerrain *terrain)
{
	this->terrain = terrain;

	motion_state = new btDefaultMotionState(btTransform(btQuaternion(0.0, 0.0, 0.0, 1), btVector3(0.0, 0.0, 0.0)));
	rigid_body = new btRigidBody(0.0, motion_state, terrain->GetCollisionShape(), btVector3(0.0, 0.0, 0.0));
	rigid_body->setRestitution(0.0);
}

void tTerrainObject::GeometryPass(tDeferredRenderer *renderer, bool cube_map_reflection_enabled)
{
	terrain->Paint(renderer);
}

tAABB tTerrainObject::GetAABB(void)
{
	return terrain->GetAABB();
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
