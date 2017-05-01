#include "towerengine.h"


tVector &tVector::operator*=(tMatrix3 m)
{
	return *this = m.ApplyToVector(*this);
}

tVector &tVector::operator*=(tTransform t)
{
	return *this = t.ApplyToVector(*this);
}

tVector operator+(const tVector &a, const tVector &b)
{
	tVector r;

	r.x = a.x + b.x;
	r.y = a.y + b.y;
	r.z = a.z + b.z;
	return r;
}

tVector operator-(const tVector &a, const tVector &b)
{
	tVector r;

	r.x = a.x - b.x;
	r.y = a.y - b.y;
	r.z = a.z - b.z;
	return r;
}

tVector operator*(const tVector &a, float scalar)
{
	tVector r;

	r.x = a.x * scalar;
	r.y = a.y * scalar;
	r.z = a.z * scalar;
	return r;
}

tVector operator*(float scalar, const tVector &a)
{
	tVector r;

	r.x = scalar * a.x;
	r.y = scalar * a.y;
	r.z = scalar * a.z;
	return r;
}

tVector operator*(const tVector &a, const tVector &b)
{
	tVector r;

	r.x = a.x * b.x;
	r.y = a.y * b.y;
	r.z = a.z * b.z;
	return r;
}

int operator==(const tVector a, const tVector b)
{
    if(a.x == b.x && a.y == b.y && a.z == b.z)
        return 1;
    return 0;
}

float Dot(const tVector &a, const tVector &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
/* Kreuzprodukt ist der Vektor, der auf a und b senkrecht steht.
 * Die L�nge des Vektors entspricht der Fl�che des Parallelogramms aus a und b
 */

tVector Cross(const tVector &a, const tVector &b)
{
	tVector ret;

	ret.x = a.y * b.z - a.z * b.y;
	ret.y = a.z * b.x - a.x * b.z;
	ret.z = a.x * b.y - a.y * b.x;

	return ret;
}

tVector tVec(const tVector2 &v, float y)
{
	tVector r;

	r.x = v.x;
	r.y = y;
	r.z = -v.y;

	return r;

}

tVector Mix(const tVector v1, const tVector v2, float mix)
{
	tVector r;

	if(mix < 0.0)
		mix = 0.0;
	if(mix > 1.0)
		mix = 1.0;
	
	r = v1 + (v2 - v1) * mix;
	return r;
}

tVector Mix(const tVector v[], const float a[], int count, float mix)
{
	int area = 0;
	int i;

	float d;
	float m;

	if(mix < a[0])
		return v[0];
	else if(mix > a[count - 1])
		return v[count - 1];

	for(i=0; i<count-1; i++)
	{
		if(mix > a[i] && mix <= a[i + 1])
		{
			area = i;
			break;
		}
	}
	
	d = a[area + 1] - a[area];
	m = mix - a[area];
	m /= d;
	return Mix(v[area], v[area + 1], m);
}