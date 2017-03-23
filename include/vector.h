#ifndef _VECTOR_H
#define _VECTOR_H

struct tMatrix3;
class tTransform;

struct tVector
{
	union
	{
		float v[3];
		struct
		{
			float x;
			float y;
			float z;
		};
		struct
		{
			float r;
			float g;
			float b;
		};
	};



	tVector2 xy(void) { return tVec(x, y); };
	tVector2 xz(void) { return tVec(x, z); };
	tVector2 yx(void) { return tVec(y, x); };
	tVector2 yz(void) { return tVec(y, z); };
	tVector2 zx(void) { return tVec(z, x); };
	tVector2 zy(void) { return tVec(z, y); };

	void Set(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
	float Len(void) const		{ return sqrt(x * x + y * y + z * z); }
	float SquaredLen(void) const		{ return x * x + y * y + z * z; }
	void Normalize(void)		{ float l; l = 1.0f / Len(); x *= l; y *= l; z *= l; }

	tVector Normalized()		{ tVector r = *this; r.Normalize(); return r; }

	tVector operator -(void) const	{tVector r; r.x = -x, r.y = -y, r.z = -z; return r; }
	operator tVector2() const	{tVector2 r; r.x = x; r.y = z; return r; }

	tVector &operator+=(const tVector o)	{ x += o.x; y += o.y; z += o.z; return *this; }
	tVector &operator-=(const tVector o)	{ x -= o.x; y -= o.y; z -= o.z; return *this; }
	tVector &operator*=(const tVector o)	{ x *= o.x; y *= o.y; z *= o.z; return *this; }
	tVector &operator*=(float f)			{ x *= f; y *= f; z *= f; return *this; }
	tVector &operator/=(float f)			{ x /= f; y /= f; z /= f; return *this; }
	tVector &operator*=(tMatrix3 m);
	tVector &operator*=(tTransform t);

	tVector &operator=(const btVector3 &a)	{ x = a.x(); y = a.y(); z = a.z(); return *this; }
};

inline tVector tVec(float x, float y, float z)
{
	tVector v;

	v.x = x;
	v.y = y;
	v.z = z;

	return v;
}

inline tVector tVec(const btVector3 &v)
{
	return tVec(v.x(), v.y(), v.z());
}



inline btVector3 BtVec(tVector a)		{ return btVector3(a.x, a.y, a.z); }
tVector NullVec(void);
tVector operator+(const tVector &a, const tVector &b);
tVector operator-(const tVector &a, const tVector &b);
tVector operator*(const tVector &a, float scalar);
tVector operator*(float scalar, const tVector &a);
tVector operator*(const tVector &a, const tVector &b);
int operator==(const tVector a, const tVector b);

double Dot(const tVector &a, const tVector &b);
tVector Cross(const tVector &a, const tVector &b);
tVector tVec(const tVector2 &v, float y);
tVector Rotate(const tVector &vec, const tVector &axis, float angle);
double Det(const tVector &v1, const tVector &v2, const tVector &v3);
tVector Mix(const tVector v1, const tVector v2, float mix);
tVector Mix(const tVector v[], const float a[], int count, float mix);


int IsInside(tVector2 c[4], const tVector2 &p);
void DistToPlane(const tVector &a, const tVector &b, const tVector &c, const tVector &point, double *dist, tVector *dir);
void DistToPlane(const tVector &a, const tVector &b, const tVector &c, const tVector &d, const tVector &point, double *min_dist, tVector *min_dir);

tVector PointToLine(tVector p, tVector r, tVector n);
float DistToLine(tVector p, tVector r, tVector n);

int PointInRange(tVector p, tVector s, tVector b);

//tVector RandVec(void);

#endif
