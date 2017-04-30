
#ifndef _TOWERENGINE_VRDEMO_VRCONTROLLER_H
#define _TOWERENGINE_VRDEMO_VRCONTROLLER_H

#include <towerengine.h>
#include <openvr.h>

class VRPlayer;

class VRController
{
	private:
		tWorld *world;
		VRPlayer *player;

		vr::VRControllerState_t last_state;
		vr::VRControllerState_t state;

		tTransform transform;

		tMeshObject *render_model_object;
		tLineObject *teleport_line_object;

		bool GetButtonDown(vr::EVRButtonId button);
		bool GetButtonUp(vr::EVRButtonId button);
		bool GetButtonState(vr::EVRButtonId button);

	public:
		VRController(tWorld *world, VRPlayer *vr_player);
		virtual ~VRController();

		void UpdateController(vr::TrackedDevicePose_t pose, vr::VRControllerState_t state, tMesh *mesh);

		void Update();

		void RemoveFromWorld();
};

#endif