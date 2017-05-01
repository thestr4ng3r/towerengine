
#include "common.h"
#include "vrplayer.h"
#include "vrcontroller.h"

#include <vr_context_openvr.h>


VRController::VRController(tWorld *world, VRPlayer *vr_player)
{
	this->world = world;
	this->player = vr_player;

	render_model_object = new tMeshObject(0);
	world->AddObject(render_model_object);

	teleport_line_object = new tLineObject(tVec(0.0f, 0.0f, 0.0f), tVec(0.0f, 0.0f, 0.0f));
	world->AddObject(teleport_line_object);

	memset(&pose, 0, sizeof(pose));
	memset(&last_state, 0, sizeof(last_state));
	memset(&state, 0, sizeof(state));


	pick_collision_shape = new btBoxShape(btVector3(0.05f, 0.05f, 0.05f));
	pick_ghost_object = new btPairCachingGhostObject();
	pick_ghost_object->setCollisionShape(pick_collision_shape);
	pick_ghost_object->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	world->GetDynamicsWorld()->addCollisionObject(pick_ghost_object, btBroadphaseProxy::SensorTrigger, MASK_PICKABLE);

	pick_rigid_body_motion_state = new btDefaultMotionState();
	pick_rigid_body = new btRigidBody(0.0f, pick_rigid_body_motion_state, 0);
	pick_rigid_body->setActivationState(DISABLE_DEACTIVATION);
	world->GetDynamicsWorld()->addRigidBody(pick_rigid_body, btBroadphaseProxy::SensorTrigger, 0);

	pick_constraint = 0;
	current_picked_object = 0;
}

VRController::~VRController()
{
	delete teleport_line_object;
	delete render_model_object;

	delete pick_ghost_object;
	delete pick_collision_shape;

	delete pick_rigid_body;
	delete pick_rigid_body_motion_state;
	delete pick_constraint;
}

void VRController::UpdateController(vr::TrackedDeviceIndex_t device_index,
									vr::TrackedDevicePose_t pose,
									vr::VRControllerState_t state,
									tMesh *mesh)
{
	this->device_index = device_index;

	this->pose = pose;

	last_state = this->state;
	this->state = state;

	tMatrix4 mat = tMatrix4FromOpenVR(pose.mDeviceToAbsoluteTracking);
	float *mat_v = mat.GetData();

	tMatrix3 basis = tMatrix3(tVec(mat_v[0], mat_v[1], mat_v[2]),
		tVec(mat_v[4], mat_v[5], mat_v[6]),
		tVec(mat_v[8], mat_v[9], mat_v[10]));

	tVector pos = player->GetOrigin() + tVec(mat_v[3], mat_v[7], mat_v[11]);

	transform = tTransform(basis, pos);

	render_model_object->SetTransform(transform);
	render_model_object->SetMesh(mesh);
}


void VRController::Update()
{
	UpdatePicking();
	UpdateTeleport();

	if(GetButtonDown(vr::k_EButton_ApplicationMenu))
		bullet_debug_object->SetEnabled(!bullet_debug_object->GetEnabled());
}


void VRController::UpdatePicking()
{
	pick_ghost_object->setWorldTransform(transform.ToBtTransform());
	pick_rigid_body->setWorldTransform(transform.ToBtTransform());

	btDynamicsWorld *dynamics_world = world->GetDynamicsWorld();

	btManifoldArray manifolds;
	btBroadphasePairArray &pairs = pick_ghost_object->getOverlappingPairCache()->getOverlappingPairArray();
	auto pairs_count = pairs.size();

	btRigidBody *contact_object = 0;

	// see http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Collision_Callbacks_and_Triggers
	for(int i=0; i<pairs_count; i++)
	{
		manifolds.clear();
		const btBroadphasePair &pair = pairs.at(i);
		btBroadphasePair *collision_pair = dynamics_world->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);

		if(!collision_pair)
			continue;

		if(collision_pair->m_algorithm)
			collision_pair->m_algorithm->getAllContactManifolds(manifolds);

		for(int j=0; j<manifolds.size(); j++)
		{
			btPersistentManifold *manifold = manifolds[i];

			if(manifold->getNumContacts() == 0)
				continue;

			bool is_first_body = manifold->getBody0() == pick_ghost_object;
			const btCollisionObject *other = is_first_body ? manifold->getBody1() : manifold->getBody0();
			if(!other->getUserPointer())
				continue;
			contact_object = reinterpret_cast<btRigidBody *>(other->getUserPointer());
			break;
		}

		if(contact_object)
			break;
	}


	// pick up
	if(contact_object
	   && GetButtonDown(vr::k_EButton_SteamVR_Trigger)
	   && !pick_constraint)
	{
		btTransform ta = btTransform::getIdentity();
		btTransform tb = btTransform::getIdentity();
		pick_constraint = new btFixedConstraint(*pick_rigid_body, *contact_object, ta, tb);
		dynamics_world->addConstraint(pick_constraint);
		contact_object->activate();
		current_picked_object = contact_object;
	}

	// drop
	if(GetButtonUp(vr::k_EButton_SteamVR_Trigger)
	   && pick_constraint)
	{
		dynamics_world->removeConstraint(pick_constraint);
		delete pick_constraint;
		pick_constraint = 0;
		current_picked_object->setLinearVelocity(btVector3(pose.vVelocity.v[0], pose.vVelocity.v[1], pose.vVelocity.v[2]));
		current_picked_object->setAngularVelocity(btVector3(pose.vAngularVelocity.v[0], pose.vAngularVelocity.v[1], pose.vAngularVelocity.v[2]));
		current_picked_object = 0;
	}
}


void VRController::UpdateTeleport()
{
	btDynamicsWorld *dynamics_world = world->GetDynamicsWorld();

	tVector aim_from = transform.GetPosition();
	tVector aim_to = transform.GetPosition() + transform.GetBasis().ApplyToVector(tVec(0.0f, 0.0f, -1.0f)) * 10.0f;

	teleport_line_object->SetEnabled(GetButtonState(vr::k_EButton_SteamVR_Touchpad));
	if(teleport_line_object->GetEnabled())
		teleport_line_object->SetPositions(aim_from, aim_to);

	if(GetButtonUp(vr::k_EButton_SteamVR_Touchpad))
	{
		btCollisionWorld::ClosestRayResultCallback ray_callback(BtVec(aim_from), BtVec(aim_to));
		ray_callback.m_collisionFilterMask = MASK_TELEPORT;
		dynamics_world->rayTest(BtVec(aim_from), BtVec(aim_to), ray_callback);

		if (ray_callback.hasHit())
			player->Teleport(tVec(ray_callback.m_hitPointWorld));
	}
}


bool VRController::GetButtonDown(vr::EVRButtonId button)
{
	uint64_t mask = vr::ButtonMaskFromId(button);
	return !(last_state.ulButtonPressed & mask) && (state.ulButtonPressed & mask);
}

bool VRController::GetButtonUp(vr::EVRButtonId button)
{
	uint64_t mask = vr::ButtonMaskFromId(button);
	return (last_state.ulButtonPressed & mask) && !(state.ulButtonPressed & mask);
}

bool VRController::GetButtonState(vr::EVRButtonId button)
{
	uint64_t mask = vr::ButtonMaskFromId(button);
	return (state.ulButtonPressed & mask) != 0;
}