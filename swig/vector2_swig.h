#ifndef _VECTOR2_H
#define _VECTOR2_H

struct tVector;

struct tVector2
{
	float x;
	float y;

	void Set(float _x, float _y);
	float Len(void) const;
	float SquaredLen(void) const;
	void Normalize(void);
	tVector2 &operator += (const tVector2 o);
	tVector2 &operator -= (const tVector2 o);
	tVector2 &operator *= (float f);
	tVector2 &operator /= (const tVector2 o);
	tVector2 &operator *= (const tVector2 o);
};

tVector2 tVec(float x, float y);
tVector2 operator+(const tVector2 &a, const tVector2 &b);
tVector2 operator-(const tVector2 &a, const tVector2 &b);
tVector2 operator*(const tVector2 &a, float scalar);
tVector2 operator*(float scalar, const tVector2 &a);
int operator==(const tVector2 a, const tVector2 b);
double Dot(const tVector2 &a, const tVector2 &b);

#endif
