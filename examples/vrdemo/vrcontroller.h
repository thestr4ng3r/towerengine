
#ifndef _TOWERENGINE_VRDEMO_VRCONTROLLER_H
#define _TOWERENGINE_VRDEMO_VRCONTROLLER_H

#include <towerengine.h>
#include <openvr.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

class VRPlayer;

class VRController
{
	private:
		tWorld *world;
		VRPlayer *player;

		vr::TrackedDeviceIndex_t device_index;
		vr::TrackedDevicePose_t pose;
		vr::VRControllerState_t last_state;
		vr::VRControllerState_t state;

		tTransform transform;

		tMeshObject *render_model_object;
		tLineObject *teleport_line_object;

		btCollisionShape *pick_collision_shape;
		btPairCachingGhostObject *pick_ghost_object;

		btDefaultMotionState *pick_rigid_body_motion_state;
		btRigidBody *pick_rigid_body;
		btRigidBody *current_picked_object;
		btFixedConstraint *pick_constraint;

		bool GetButtonDown(vr::EVRButtonId button);
		bool GetButtonUp(vr::EVRButtonId button);
		bool GetButtonState(vr::EVRButtonId button);

		void UpdatePicking();
		void UpdateTeleport();

	public:
		VRController(tWorld *world, VRPlayer *vr_player);
		virtual ~VRController();

		void UpdateController(vr::TrackedDeviceIndex_t device_index,
							  vr::TrackedDevicePose_t pose,
							  vr::VRControllerState_t state,
							  tMesh *mesh);

		void Update();
};

#endif