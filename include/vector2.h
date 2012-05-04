#ifndef _CVECTOR2_H
#define _CVECTOR2_H

struct CVector;

struct CVector2
{
	union
	{
		struct
		{
			float x;
			float y;
		};
		float v[2];
	};

	void Set(float _x, float _y)	{ x = _x; y = _y;};
	float Len(void) const		{ return sqrt(x * x + y * y); };
	float SquaredLen(void) const		{ return x * x + y * y; };
	void Normalize(void)		{ float l; l = 1.0 / Len(); x *= l; y *= l; };
	CVector2 &operator += (const CVector2 o) { x+=o.x; y+=o.y; return *this; };
	CVector2 &operator -= (const CVector2 o) { x-=o.x; y-=o.y; return *this; };
	CVector2 &operator *= (float f) { x*=f; y*=f; return *this; };
        CVector2 &operator /= (const CVector2 o) { x/=o.x; y/=o.y; return *this; };
        CVector2 &operator *= (const CVector2 o) { x*=o.x; y*=o.y; return *this; };

	CVector2 &operator=(const CVector &v);
};

CVector2 Vec(float x, float y);
CVector2 operator+(const CVector2 &a, const CVector2 &b);
CVector2 operator-(const CVector2 &a, const CVector2 &b);
CVector2 operator*(const CVector2 &a, float scalar);
CVector2 operator*(float scalar, const CVector2 &a);
int operator==(const CVector2 a, const CVector2 b);
double Dot(const CVector2 &a, const CVector2 &b);

#endif
