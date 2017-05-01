
#include <towerengine.h>

#include "vrplayer.h"

VRPlayer::VRPlayer(tVector origin)
{
	this->origin = origin;
	this->base_position = origin;

	teleport = false;
	teleport_dst = tVec(0.0f, 0.0f, 0.0f);
}

VRPlayer::~VRPlayer()
{
}

void VRPlayer::UpdatePosition(tVector headet_position, tVector headset_dir)
{
	base_position = headet_position;
	base_position.y = origin.y;
	height = headet_position.y - origin.y;

	direction = headset_dir;
}

void VRPlayer::Teleport(tVector target_pos)
{
	tVector current_dir = base_position - origin;
	origin = target_pos - current_dir;
}

void VRPlayer::Update()
{
}