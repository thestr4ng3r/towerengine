
#include "towerengine.h"

tCamera::tCamera(void)
{
	pos = Vec(0.0, 0.0, 0.0);
	dir = Vec(0.0, 0.0, -1.0);
	up = Vec(0.0, 1.0, 0.0);
	aspect = 0.75;
	angle = 60.0;
	near_clip = 0.1;
	far_clip = 300.0;
}

tVector *tCamera::GetRelativeFrustumCorners(float near_clip, float far_clip)
{
	tVector *c = new tVector[8];
	float vert, horz;
	tVector hvec = Cross(dir, up);
	tVector vvec = Cross(hvec, dir);
	hvec.Normalize();
	vvec.Normalize();

	vert = tan(degtorad(angle) / 2.0);
	horz = vert * aspect;

	c[0] = c[1] = c[2] = c[3] = dir;

	c[0] += vert * vvec - hvec * horz;
	c[1] += -vert * vvec - hvec * horz;
	c[2] += -vert * vvec + hvec * horz;
	c[3] += vert * vvec + hvec * horz;

	c[4] = c[0] * far_clip;
	c[5] = c[1] * far_clip;
	c[6] = c[2] * far_clip;
	c[7] = c[3] * far_clip;

	c[0] *= near_clip;
	c[1] *= near_clip;
	c[2] *= near_clip;
	c[3] *= near_clip;

	return c;
}

tVector *tCamera::GetViewRays(void)
{
	tVector *c = new tVector[4];
	float vert, horz;

	vert = tan(degtorad(angle) / 2.0);
	horz = vert * aspect;

	c[0] = Vec(vert, -horz, 1.0);
	c[1] = Vec(-vert, -horz, 1.0);
	c[2] = Vec(-vert, horz, 1.0);
	c[3] = Vec(vert, horz, 1.0);

	return c;
}

tVector tCamera::GetScreenRay(float x, float y)
{
	float vert, horz;

	vert = tan(degtorad(angle) / 2.0);
	horz = vert * aspect;

	tVector r = Vec(x * horz, y * vert, 1.0);
	r *= far_clip;

	tVector hvec = Cross(dir, up);
	tVector vvec = Cross(hvec, dir);
	hvec.Normalize();
	vvec.Normalize();

	r = r.x * hvec + r.y * vvec + r.z * dir;

	return r;

}

tVector2 tCamera::GetProjectedPoint(tVector point)
{
	tVector hvec = Cross(dir, up);
	tVector vvec = Cross(hvec, dir);
	hvec.Normalize();
	vvec.Normalize();

	float dist = Dot(dir, point - pos);
	tVector proj_center = pos + dir * dist;

	float tang = tan(degtorad(angle) * 0.5);
	float height = dist * tang;
	float width = height * aspect;

	tVector2 proj;
	proj.x = Dot(point - proj_center, hvec) / width;
	proj.y = Dot(point - proj_center, vvec) / height;

	return proj;
}


void tCamera::CalculateFrustumPlanes(void)
{
	tVector *points = frustum_planes_points;
	tVector *normals = frustum_planes_normals;

	tVector hvec = Cross(dir, up);
	tVector vvec = Cross(hvec, dir);
	hvec.Normalize();
	vvec.Normalize();

	float tang = tan(degtorad(angle) * 0.5);
	float nh = near_clip * tang;
	float nw = nh * aspect;

	tVector aux;

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

void tCamera::InitCulling(void)
{
	CalculateFrustumPlanes();
}

bool tCamera::TestPointCulling(tVector point)
{
	for(int i=0; i<6; i++)
	{
		if(Dot(point - frustum_planes_points[i], frustum_planes_normals[i]) < 0.0)
			return true;
	}
	return false;
}

bool tCamera::TestSphereCulling(tVector center, float radius)
{
	for(int i=0; i<6; i++)
	{
		if(Dot(center - frustum_planes_points[i], frustum_planes_normals[i]) < -radius)
			return true;
	}
	return false;
}

bool tCamera::TestBoundingBoxCulling(tBoundingBox b)
{
	tVector p, n;
	tVector normal;

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

void tCamera::CalculateModelViewProjectionMatrix(void)
{
	tVector to = pos + dir;
	modelview_matrix.SetLookAt(pos, to, up);
	projection_matrix.SetPerspective(angle, aspect, near_clip, far_clip);
	modelview_projection_matrix = projection_matrix * modelview_matrix;
}





