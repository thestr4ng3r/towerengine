
#ifndef _TOWERENGINE_VRDEMO_VRPLAYER_H
#define _TOWERENGINE_VRDEMO_VRPLAYER_H

#include <towerengine.h>

class VRPlayer
{
	private:
		tVector origin;

		tVector base_position;
		tVector direction;
		float height;

		bool teleport;
		tVector teleport_dst;

	public:
		VRPlayer(tVector origin);
		~VRPlayer();

		tVector GetOrigin() const			{ return origin; }
		tVector GetBasePosition() const		{ return base_position; }

		void UpdatePosition(tVector headet_position, tVector headset_dir);
		void Teleport(tVector target_pos);

		void Update();
};

#endif