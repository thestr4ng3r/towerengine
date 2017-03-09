#ifndef _VECTOR_H
#define _VECTOR_H

struct tVector
{
	float x;
	float y;
	float z;

	tVector2 xy(void);
	tVector2 xz(void);
	tVector2 yx(void);
	tVector2 yz(void);
	tVector2 zx(void);
	tVector2 zy(void);

	void Set(float _x, float _y, float _z);
	float Len(void);
	float SquaredLen(void);
	void Normalize(void);

	tVector operator -(void) const;

	tVector &operator+=(const tVector o);
	tVector &operator-=(const tVector o);
	tVector &operator*=(const tVector o);
	tVector &operator*=(float f);
	tVector &operator/=(float f);
	tVector &operator*=(tMatrix3 m);
	tVector &operator*=(tTransform t);
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
