
#include "towerengine.h"


CBoundingBox::CBoundingBox(CVector a, CVector b)
{
	SetBounds(a, b);
}

CBoundingBox::CBoundingBox(void)
{
	minv = Vec(INFINITY, INFINITY, INFINITY);
	maxv = Vec(-INFINITY, -INFINITY, -INFINITY);
}

void CBoundingBox::SetBounds(CVector a, CVector b)
{
	minv.x = min(a.x, b.x);
	minv.y = min(a.y, b.y);
	minv.z = min(a.z, b.z);

	maxv.x = max(a.x, b.x);
	maxv.y = max(a.y, b.y);
	maxv.z = max(a.z, b.z);
}

void CBoundingBox::AddPoint(CVector p)
{
	minv.x = min(minv.x, p.x);
	minv.y = min(minv.y, p.y);
	minv.z = min(minv.z, p.z);

	maxv.x = max(maxv.x, p.x);
	maxv.y = max(maxv.y, p.y);
	maxv.z = max(maxv.z, p.z);
}

CVector *CBoundingBox::GetCornerPoints(void)
{
	CVector *p = new CVector[8];

	p[0] = Vec(minv.x, maxv.y, minv.z);
	p[1] = Vec(minv.x, maxv.y, maxv.z);
	p[2] = Vec(maxv.x, maxv.y, maxv.z);
	p[3] = Vec(maxv.x, maxv.y, minv.z);
	p[4] = Vec(minv.x, minv.y, minv.z);
	p[5] = Vec(minv.x, minv.y, maxv.z);
	p[6] = Vec(maxv.x, minv.y, maxv.z);
	p[7] = Vec(maxv.x, minv.y, minv.z);

	return p;
}

CBoundingBox operator+(CBoundingBox a, const CVector &b)
{
	return CBoundingBox(a.GetMin() + b, a.GetMax() + b);
}
