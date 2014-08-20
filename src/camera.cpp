
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


void CCamera::CalculateFrustumPlanes(void)
{
	CVector *points = frustum_planes_points;
	CVector *normals = frustum_planes_normals;

	CVector hvec = Cross(dir, up);
	CVector vvec = Cross(hvec, dir);
	hvec.Normalize();
	vvec.Normalize();

	float tang = tan(degtorad(angle) * 0.5);
	float nh = near_clip * tang;
	float nw = nh * aspect;

	CVector aux;

	points[0] = pos + dir * near_clip;
	normals[0] = dir;

	points[1] = pos + dir * far_clip;
	normals[1] = -dir;

	aux = (points[0] + vvec*nh) - pos;
	aux.Normalize();
	normals[2] = Cross(aux, hvec);
	points[2] = points[0] + vvec*nh;

	aux = (points[0] - vvec*nh) - pos;
	aux.Normalize();
	normals[3] = Cross(hvec, aux);
	points[3] = points[0] - vvec*nh;

	aux = (points[0] - hvec*nw) - pos;
	aux.Normalize();
	normals[4] = Cross(aux, vvec);
	points[4] = points[0] - hvec*nw;

	aux = (points[0] + hvec*nw) - pos;
	aux.Normalize();
	normals[5] = Cross(vvec, aux);
	points[5] = points[0] + hvec*nw;
}

bool CCamera::TestPointCulling(CVector point)
{
	for(int i=0; i<6; i++)
	{
		if(Dot(point - frustum_planes_points[i], frustum_planes_normals[i]) < 0.0)
			return true;
	}
	return false;
}

bool CCamera::TestSphereCulling(CVector center, float radius)
{
	for(int i=0; i<6; i++)
	{
		if(Dot(center - frustum_planes_points[i], frustum_planes_normals[i]) < -radius)
			return true;
	}
	return false;
}

bool CCamera::TestBoundingBoxCulling(CBoundingBox b)
{
	CVector p, n;
	CVector normal;

	for(int i=0; i<6; i++)
	{
		normal = frustum_planes_normals[i];

		p = b.GetMin();
		if(normal.x >= 0)
			p.x = b.GetMax().x;
		if(normal.y >=0)
			p.y = b.GetMax().y;
		if(normal.z >= 0)
			p.z = b.GetMax().z;

		n = b.GetMax();
		if(normal.x >= 0)
			n.x = b.GetMin().x;
		if(normal.y >=0)
			n.y = b.GetMin().y;
		if(normal.z >= 0)
			n.z = b.GetMin().z;

		if((Dot(n - frustum_planes_points[i], normal) < 0.0) && (Dot(p - frustum_planes_points[i], normal) < 0.0))
			return true;
	}

	return false;
}




