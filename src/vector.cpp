#include "towerengine.h"


/* gegeben ist eine Ebene durch den Normalvektor n und den Abstand zum Ursprung d
   SetFromPlane setzt meine Koordinaten von dem zweidimensionalen Punkt p (von oben gesehen), der auf der Ebene liegt
   und erg�nzt die y-Koordinate, sodass der Ergebnispunkt auf der Ebene liegt */
void tVector::SetFromPlane(const tVector2 &p, const tVector &n, float d)
{
	x = p.x;
	y = (d - p.x * n.x - p.y * n.z) / n.y;
	z = p.y;
}

tVector &tVector::operator*=(tMatrix3 m)
{
	return *this = m.ApplyToVector(*this);
}

tVector &tVector::operator*=(tTransform t)
{
	return *this = t.ApplyToVector(*this);
}

int IsInside(tVector2 c[4], const tVector2 &p)
{
	
	int hit_count;
	int i;

	hit_count = 0;

   	for(i=0; i<4; i++)
	{
		int i1 = (i + 1) % 4;

		if((c[i].y < p.y) == (c[i1].y < p.y))
			continue;

		double sx = c[i].x + (p.y - c[i].y) / (c[i1].y - c[i].y) * (c[i1].x - c[i].x);

		if(sx > p.x)
			continue;

		hit_count++;
	}

	return hit_count % 2;
}

int PointInRange(tVector p, tVector s, tVector b)
{
	bool i;

	i = (p.x > s.x) && (p.y > s.y) && (p.z > s.z);
	i = i && (p.x < b.x) && (p.y < b.y) && (p.z < b.z);

	return (int)i;	
}

tVector PointToLine(tVector p, tVector r, tVector n)
{
	float l = Dot(p, n) - Dot(n, r);
	l /= Dot(n*n, Vec(1.0, 1.0, 1.0));
	tVector f = r + n*l;
	return f-p;
}

float DistToLine(tVector p, tVector r, tVector n)
{
	return PointToLine(p, r, n).Len();
}

void DistToPlane(const tVector &a, const tVector &b, const tVector &c, const tVector &point, double *dist, tVector *dir)
{
	tVector d = c;
	DistToPlane(a, b, c, d, point, dist, dir);
}

void DistToPlane(const tVector &a, const tVector &b, const tVector &c, const tVector &d, const tVector &point, double *mdist, tVector *dir)
{           
	tVector2 cp[4];
	tVector2 p;
	tVector normal;
	tVector proj;
	double dist;

	normal = Cross(b-a, c-a);
	normal.Normalize();
	dist = Dot(normal, point - a);

	if(dist < 0.0)
	{
		dist = -dist;
		normal = -normal;
	}

	proj = point - normal * dist;

	if(fabs(normal.x) > fabs(normal.y) && fabs(normal.x) > fabs(normal.z))
	{
		cp[0].Set(a.y, a.z);
      		cp[1].Set(b.y, b.z);
      		cp[2].Set(c.y, c.z);
      		cp[3].Set(d.y, d.z);

		p.Set(proj.y, proj.z);
	}
	else if(fabs(normal.y) > fabs(normal.z))
	{
   		cp[0].Set(a.x, a.z);
      		cp[1].Set(b.x, b.z);
      		cp[2].Set(c.x, c.z);
      		cp[3].Set(d.x, d.z);

		p.Set(proj.x, proj.z);  
	}
	else
	{
 		cp[0].Set(a.x, a.y);
      		cp[1].Set(b.x, b.y);
      		cp[2].Set(c.x, c.y);
      		cp[3].Set(d.x, d.y);

		p.Set(proj.x, proj.y);  
	}

	if(!IsInside(cp, p))
		return;

	*mdist = dist;
	*dir = normal;
}



/*#ifdef TMS_USE_LIB_3DS
CVector Vec3ds(Lib3dsVector v)
{
    return Vec(v[0], v[1], v[2]);
}
#endif

#ifdef TMS_USE_LIB_G3D
CVector VecG3D(G3DVector v[3])
{
    return Vec(v[0], v[1], v[2]);
}
#endif*/

tVector NullVec(void)
{
    return Vec(0.0, 0.0, 0.0);
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

double Dot(const tVector &a, const tVector &b)
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

tVector Vec(const tVector2 &v, float y)
{
	tVector r;

	r.x = v.x;
	r.y = y;
	r.z = -v.y;

	return r;

}

tVector Rotate(const tVector &vec, const tVector &axis, float angle)
{
	tVector a;
	tVector x;
	tVector y;
	tVector z;
	double s, c, t;

	a = axis;
	a.Normalize();

	s = sin(angle);
	c = cos(angle);
	t = 1.0 - c;

	x.x = t * a.x * a.x + c;
	x.y = t * a.x * a.y - s * a.z;
	x.z = t * a.x * a.z + s * a.y;

	y.x = t * a.x * a.y + s * a.z;
	y.y = t * a.y * a.y + c;
	y.z = t * a.y * a.z - s * a.x;

	z.x = t * a.x * a.z - s * a.y;
	z.y = t * a.y * a.z + s * a.x;
	z.z = t * a.z * a.z + c;

	return vec.x * x + vec.y * y + vec.z * z;
}

double Det(const tVector &v1, const tVector &v2, const tVector &v3)
{
	return v1.x * v2.y * v3.z + v2.x * v3.y * v1.z + v3.x * v3.y * v2.z - v1.x * v3.y * v2.z - v2.x * v1.y * v3.z - v3.x * v2.y * v1.z;
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


tVector RandVec(void)
{
	tVector r;
	tVector v;

	r = Vec((float)rand(), (float)rand(), (float)rand());
	r /= RAND_MAX;
	r *= M_PI;
	v.x = sin(r.y) * cos(r.z);
	v.y = sin(r.z) * cos(r.x);
	v.z = cos(r.y) * sin(r.x);
	v.Normalize();

	return v;
}
