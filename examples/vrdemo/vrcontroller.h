
#ifndef _TOWERENGINE_VRDEMO_VRCONTROLLER_H
#define _TOWERENGINE_VRDEMO_VRCONTROLLER_H

#include <towerengine.h>
#include <openvr.h>

class VRController
{
	private:
		tWorld *world;

		vr::TrackedDevicePose_t pose;
		vr::VRControllerState_t state;

		tMeshObject *render_model_object;
		tLineObject *teleport_line_object;

	public:
		VRController(tWorld *world);
		virtual ~VRController();

		void Update(tVector *origin, vr::TrackedDevicePose_t pose, vr::VRControllerState_t state, tMesh *mesh);

		void RemoveFromWorld();
};

#endif