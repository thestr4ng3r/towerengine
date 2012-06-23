
#include "towerengine.h"

CDirCamera::CDirCamera(void)
{
	pos = Vec(0.0, 0.0, 0.0);
	dir = Vec(0.0, 0.0, -1.0);
	top = Vec(0.0, 1.0, 0.0);
}

void CDirCamera::SetGLMatrix(void)
{
	CVector focus = pos + dir;
	gluLookAt(pos.x, pos.y, pos.z, focus.x, focus.y, focus.z, top.x, top.y, top.z);
}
