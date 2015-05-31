#ifndef _CVECTOR2_H
#define _CVECTOR2_H

struct tVector;

struct tVector2
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
	tVector2 &operator += (const tVector2 o) { x+=o.x; y+=o.y; return *this; };
	tVector2 &operator -= (const tVector2 o) { x-=o.x; y-=o.y; return *this; };
	tVector2 &operator *= (float f) { x*=f; y*=f; return *this; };
	tVector2 &operator /= (const tVector2 o) { x/=o.x; y/=o.y; return *this; };
	tVector2 &operator *= (const tVector2 o) { x*=o.x; y*=o.y; return *this; };

	tVector2 &operator=(const tVector &v);
};

tVector2 Vec(float x, float y);
tVector2 operator+(const tVector2 &a, const tVector2 &b);
tVector2 operator-(const tVector2 &a, const tVector2 &b);
tVector2 operator*(const tVector2 &a, float scalar);
tVector2 operator*(float scalar, const tVector2 &a);
int operator==(const tVector2 a, const tVector2 b);
double Dot(const tVector2 &a, const tVector2 &b);

#endif
