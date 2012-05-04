#include "towerengine.h"


CVector2 Vec(float x, float y)
{
	CVector2 v;

	v.x = x;
	v.y = y;

	return v;
}

CVector2 operator+(const CVector2 &a, const CVector2 &b)
{
	CVector2 r;

	r.x = a.x + b.x;
	r.y = a.y + b.y;
	return r;

}

CVector2 operator-(const CVector2 &a, const CVector2 &b)
{
	CVector2 r;

	r.x = a.x - b.x;
	r.y = a.y - b.y;
	return r;

}

CVector2 operator*(const CVector2 &a, float scalar)
{
	CVector2 r;

	r.x = a.x * scalar;
	r.y = a.y * scalar;
	return r;

}

CVector2 operator*(float scalar, const CVector2 &a)
{
	CVector2 r;

	r.x = scalar * a.x;
	r.y = scalar * a.y;
	return r;

}

CVector2 &CVector2::operator=(const CVector &v)
{
	x = v.x;
	y = -v.z;
	return *this;
}

int operator==(const CVector2 a, const CVector2 b)
{
    if(a.x == b.x && a.y == b.y)
        return 1;
    return 0;
}

double Dot(const CVector2 &a, const CVector2 &b)
{
	return a.x * b.x + a.y * b.y;
}
