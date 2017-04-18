
#include "common.h"
#include "vrcontroller.h"

#include <vr_context_openvr.h>



VRController::VRController(tWorld *world)
{
	this->world = world;

	render_model_object = new tMeshObject(0);
	world->AddObject(render_model_object);

	teleport_line_object = new tLineObject(tVec(0.0f, 0.0f, 0.0f), tVec(0.0f, 0.0f, 0.0f));
	world->AddObject(teleport_line_object);
}

VRController::~VRController()
{
	delete teleport_line_object;
	delete render_model_object;
}

void VRController::Update(tVector *origin, vr::TrackedDevicePose_t pose, vr::VRControllerState_t state, tMesh *mesh)
{
	vr::VRControllerState_t old_state = this->state;
	this->pose = pose;
	this->state = state;

	tMatrix4 mat = ConvertOpenVRMatrix(pose.mDeviceToAbsoluteTracking);
	float *mat_v = mat.GetData();

	tMatrix3 basis = tMatrix3(tVec(mat_v[0], mat_v[1], mat_v[2]),
		tVec(mat_v[4], mat_v[5], mat_v[6]),
		tVec(mat_v[8], mat_v[9], mat_v[10]));

	tVector pos = *origin + tVec(mat_v[3], mat_v[7], mat_v[11]);

	tTransform transform(basis, pos);

	render_model_object->SetTransform(transform);
	render_model_object->SetMesh(mesh);





	tVector aim_from = pos;
	tVector aim_to = pos + transform.GetBasis().ApplyToVector(tVec(0.0f, 0.0f, -1.0f)) * 10.0f;

	teleport_line_object->SetPositions(aim_from, aim_to);
	
	uint64_t teleport_button_mask = vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad);

	if (!(old_state.ulButtonPressed & teleport_button_mask)
		&& (state.ulButtonPressed & teleport_button_mask))
	{
		btDynamicsWorld *dynamics_world = world->GetDynamicsWorld();
		btCollisionWorld::ClosestRayResultCallback ray_callback(BtVec(aim_from), BtVec(aim_to));
		ray_callback.m_collisionFilterMask = MASK_TELEPORT;
		dynamics_world->rayTest(BtVec(aim_from), BtVec(aim_to), ray_callback);

		if (ray_callback.hasHit())
		{
			*origin = tVec(ray_callback.m_hitPointWorld);
		}
	}
}

void VRController::RemoveFromWorld()
{
	world->RemoveObject(teleport_line_object);
	world->RemoveObject(render_model_object);
}
