
#include "towerengine.h"


tBoundingBox::tBoundingBox(tVector a, tVector b)
{
	SetBounds(a, b);
}

tBoundingBox::tBoundingBox(void)
{
	minv = Vec(INFINITY, INFINITY, INFINITY);
	maxv = Vec(-INFINITY, -INFINITY, -INFINITY);
}

void tBoundingBox::SetBounds(tVector a, tVector b)
{
	minv.x = min(a.x, b.x);
	minv.y = min(a.y, b.y);
	minv.z = min(a.z, b.z);

	maxv.x = max(a.x, b.x);
	maxv.y = max(a.y, b.y);
	maxv.z = max(a.z, b.z);
}

void tBoundingBox::AddPoint(tVector p)
{
	minv.x = min(minv.x, p.x);
	minv.y = min(minv.y, p.y);
	minv.z = min(minv.z, p.z);

	maxv.x = max(maxv.x, p.x);
	maxv.y = max(maxv.y, p.y);
	maxv.z = max(maxv.z, p.z);
}

tVector *tBoundingBox::GetCornerPoints(void)
{
	tVector *p = new tVector[8];

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

tBoundingBox operator+(tBoundingBox a, const tVector &b)
{
	return tBoundingBox(a.GetMin() + b, a.GetMax() + b);
}
