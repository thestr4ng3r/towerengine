
#include "towerengine.h"

CCamera::CCamera(void)
{
	pos = Vec(0.0, 0.0, 0.0);
	dir = Vec(0.0, 0.0, -1.0);
	up = Vec(0.0, 1.0, 0.0);
	aspect = 0.75;
	angle = 60.0;
	near_clip = 0.1;
	far_clip = 300.0;
}

CVector *CCamera::GetRelativeFrustumCorners(float near, float far)
{
	CVector *c = new CVector[8];
	float vert, horz;
	CVector hvec = Cross(dir, up);
	CVector vvec = Cross(hvec, dir);
	hvec.Normalize();
	vvec.Normalize();

	vert = tan(degtorad(angle) / 2.0);
	horz = vert * aspect;

	c[0] = c[1] = c[2] = c[3] = dir;

	c[0] += vert * vvec - hvec * horz;
	c[1] += -vert * vvec - hvec * horz;
	c[2] += -vert * vvec + hvec * horz;
	c[3] += vert * vvec + hvec * horz;

	c[4] = c[0] * far;
	c[5] = c[1] * far;
	c[6] = c[2] * far;
	c[7] = c[3] * far;

	c[0] *= near;
	c[1] *= near;
	c[2] *= near;
	c[3] *= near;

	return c;
}
