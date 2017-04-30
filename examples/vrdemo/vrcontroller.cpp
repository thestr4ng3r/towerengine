
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

	memset(&last_state, 0, sizeof(last_state));
}

VRController::~VRController()
{
	delete teleport_line_object;
	delete render_model_object;
}

void VRController::UpdateController(vr::TrackedDevicePose_t pose, vr::VRControllerState_t state, tMesh *mesh)
{
	last_state = this->state;
	this->state = state;

	tMatrix4 mat = ConvertOpenVRMatrix(pose.mDeviceToAbsoluteTracking);
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
	tVector aim_from = transform.GetPosition();
	tVector aim_to = transform.GetPosition() + transform.GetBasis().ApplyToVector(tVec(0.0f, 0.0f, -1.0f)) * 10.0f;

	teleport_line_object->SetEnabled(GetButtonState(vr::k_EButton_SteamVR_Touchpad));
	if(teleport_line_object->GetEnabled())
		teleport_line_object->SetPositions(aim_from, aim_to);

	if(GetButtonUp(vr::k_EButton_SteamVR_Touchpad))
	{
		btDynamicsWorld *dynamics_world = world->GetDynamicsWorld();
		btCollisionWorld::ClosestRayResultCallback ray_callback(BtVec(aim_from), BtVec(aim_to));
		ray_callback.m_collisionFilterMask = MASK_TELEPORT;
		dynamics_world->rayTest(BtVec(aim_from), BtVec(aim_to), ray_callback);

		if (ray_callback.hasHit())
			player->Teleport(tVec(ray_callback.m_hitPointWorld));
	}
}

void VRController::RemoveFromWorld()
{
	world->RemoveObject(teleport_line_object);
	world->RemoveObject(render_model_object);
}


bool VRController::GetButtonDown(vr::EVRButtonId button)
{
	uint64_t mask = vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad);
	return !(last_state.ulButtonPressed & mask) && (state.ulButtonPressed & mask);
}

bool VRController::GetButtonUp(vr::EVRButtonId button)
{
	uint64_t mask = vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad);
	return (last_state.ulButtonPressed & mask) && !(state.ulButtonPressed & mask);
}

bool VRController::GetButtonState(vr::EVRButtonId button)
{
	uint64_t mask = vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad);
	return (state.ulButtonPressed & mask) != 0;
}