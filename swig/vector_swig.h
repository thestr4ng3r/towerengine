#ifndef _VECTOR_H
#define _VECTOR_H

struct tVector
{
	float x;
	float y;
	float z;
};

inline tVector Vec(float x, float y, float z)
{
	tVector v;

	v.x = x;
	v.y = y;
	v.z = z;

	return v;
}

inline tVector Vec(const btVector3 &v)
{
	return Vec(v.x(), v.y(), v.z());
}

#endif
