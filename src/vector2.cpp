#include "towerengine.h"


tVector2 Vec(float x, float y)
{
	tVector2 v;

	v.x = x;
	v.y = y;

	return v;
}

tVector2 operator+(const tVector2 &a, const tVector2 &b)
{
	tVector2 r;

	r.x = a.x + b.x;
	r.y = a.y + b.y;
	return r;

}

tVector2 operator-(const tVector2 &a, const tVector2 &b)
{
	tVector2 r;

	r.x = a.x - b.x;
	r.y = a.y - b.y;
	return r;

}

tVector2 operator*(const tVector2 &a, float scalar)
{
	tVector2 r;

	r.x = a.x * scalar;
	r.y = a.y * scalar;
	return r;

}

tVector2 operator*(float scalar, const tVector2 &a)
{
	tVector2 r;

	r.x = scalar * a.x;
	r.y = scalar * a.y;
	return r;

}

int operator==(const tVector2 a, const tVector2 b)
{
    if(a.x == b.x && a.y == b.y)
        return 1;
    return 0;
}

double Dot(const tVector2 &a, const tVector2 &b)
{
	return a.x * b.x + a.y * b.y;
}
