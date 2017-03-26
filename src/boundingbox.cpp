
#include "towerengine.h"

using namespace std;

tBoundingBox::tBoundingBox(tVector a, tVector b)
{
	SetBounds(a, b);
}

tBoundingBox::tBoundingBox(void)
{
	minv = tVec(INFINITY, INFINITY, INFINITY);
	maxv = tVec(-INFINITY, -INFINITY, -INFINITY);
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

void tBoundingBox::GetCornerPoints(tVector *p)
{
	p[0] = tVec(minv.x, maxv.y, minv.z);
	p[1] = tVec(minv.x, maxv.y, maxv.z);
	p[2] = tVec(maxv.x, maxv.y, maxv.z);
	p[3] = tVec(maxv.x, maxv.y, minv.z);
	p[4] = tVec(minv.x, minv.y, minv.z);
	p[5] = tVec(minv.x, minv.y, maxv.z);
	p[6] = tVec(maxv.x, minv.y, maxv.z);
	p[7] = tVec(maxv.x, minv.y, minv.z);
}

bool tBoundingBox::ContainsPoint(tVector p)
{
	return p.x >= minv.x && p.x <= maxv.x
			&& p.y >= minv.y && p.y <= maxv.y
			&& p.z >= minv.z && p.z <= maxv.z;
}

tVector tBoundingBox::GetNormalizedDistanceToCenter(tVector pos)
{
	tVector center = (maxv + minv) * 0.5;
	tVector dist = pos - center;
	dist.x /= maxv.x - center.x;
	dist.y /= maxv.y - center.y;
	dist.z /= maxv.z - center.z;
	return dist;
}

float tBoundingBox::GetNormalizedBoxDistanceToCenter(tVector pos)
{
	tVector dist = GetNormalizedDistanceToCenter(pos);
	return max(max(abs(dist.x), abs(dist.y)), abs(dist.z));
}

tBoundingBox operator+(tBoundingBox a, const tVector &b)
{
	return tBoundingBox(a.GetMin() + b, a.GetMax() + b);
}
