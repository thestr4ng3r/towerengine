
#include "towerengine.h"

using namespace std;

tAABB::tAABB(tVector a, tVector b, bool infinite)
{
	SetBounds(a, b);
	this->infinite = infinite;
}

tAABB::tAABB(bool infinite)
{
	minv = tVec(0.0f, 0.0f, 0.0f);
	maxv = tVec(0.0f, 0.0f, 0.0f);
	this->infinite = infinite;
}

tAABB::tAABB(void)
{
	minv = tVec(0.0f, 0.0f, 0.0f);
	maxv = tVec(0.0f, 0.0f, 0.0f);
	this->infinite = true;
}

void tAABB::SetBounds(tVector a, tVector b)
{
	minv.x = min(a.x, b.x);
	minv.y = min(a.y, b.y);
	minv.z = min(a.z, b.z);

	maxv.x = max(a.x, b.x);
	maxv.y = max(a.y, b.y);
	maxv.z = max(a.z, b.z);
}

void tAABB::AddPoint(tVector p)
{
	minv.x = min(minv.x, p.x);
	minv.y = min(minv.y, p.y);
	minv.z = min(minv.z, p.z);

	maxv.x = max(maxv.x, p.x);
	maxv.y = max(maxv.y, p.y);
	maxv.z = max(maxv.z, p.z);
}

void tAABB::GetCornerPoints(tVector *p)
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

bool tAABB::ContainsPoint(tVector p)
{
	return infinite ||
			(p.x >= minv.x && p.x <= maxv.x
			&& p.y >= minv.y && p.y <= maxv.y
			&& p.z >= minv.z && p.z <= maxv.z);
}

tVector tAABB::GetNormalizedDistanceToCenter(tVector pos)
{
	tVector center = (maxv + minv) * 0.5;
	tVector dist = pos - center;
	dist.x /= maxv.x - center.x;
	dist.y /= maxv.y - center.y;
	dist.z /= maxv.z - center.z;
	return dist;
}

float tAABB::GetNormalizedBoxDistanceToCenter(tVector pos)
{
	tVector dist = GetNormalizedDistanceToCenter(pos);
	return max(max(abs(dist.x), abs(dist.y)), abs(dist.z));
}

tAABB operator+(tAABB a, const tVector &b)
{
	return tAABB(a.GetMin() + b, a.GetMax() + b);
}
