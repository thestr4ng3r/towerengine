#ifndef _VECTOR_H
#define _VECTOR_H

#define pvec(v) v.x, v.y, v.z
#define pvec2(v) v.x, v.y

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

	tVector2 xy(void) { return Vec(x, y); };
	tVector2 xz(void) { return Vec(x, z); };
	tVector2 yx(void) { return Vec(y, x); };
	tVector2 yz(void) { return Vec(y, z); };
	tVector2 zx(void) { return Vec(z, x); };
	tVector2 zy(void) { return Vec(z, y); };

	void Set(float _x, float _y, float _z) { x = _x; y = _y; z = _z; };
	float Len(void) const		{ return sqrt(x * x + y * y + z * z); };
   float SquaredLen(void) const		{ return x * x + y * y + z * z; };
	void Normalize(void)		{ float l; l = 1.0 / Len(); x *= l; y *= l; z *= l; };
	void PutToGL(void) const	{ glVertex3fv(v); };
	void NormalToGL(void) const	{ glNormal3fv(v); };
	void TexcoordToGL(void) const	{ glMultiTexCoord3fv(GL_TEXTURE0, v); };
	void TangXToGL(void) const	{ glMultiTexCoord3fv(GL_TEXTURE1, v); };
	void TangYToGL(void) const	{ glMultiTexCoord3fv(GL_TEXTURE2, v); };
	void AttrToGL(int which) const             { glVertexAttrib3fv(which, v); };
	void SetFromPlane(const tVector2 &p, const tVector &n, float d);
	tVector operator -(void) const	{tVector r; r.x = -x, r.y = -y, r.z = -z; return r; };
	operator tVector2() const	{tVector2 r; r.x = x; r.y = z; return r; };

	tVector &operator+=(const tVector o)	{x += o.x; y += o.y; z += o.z; return *this;};
	tVector &operator-=(const tVector o)	{x -= o.x; y -= o.y; z -= o.z; return *this;};
	tVector &operator*=(const tVector o)	{x *= o.x; y *= o.y; z *= o.z; return *this;};
	tVector &operator*=(float f)			{x *= f; y *= f; z *= f; return *this;};
	tVector &operator/=(float f)			{x /= f; y /= f; z /= f; return *this;};
	#ifdef TMS_USE_LIB_3DS
        tVector &operator=(Lib3dsVector o)      {x = o[0]; y = o[1]; z = o[2]; return *this;};
	#endif

	tVector &operator=(const btVector3 a)	{ x = a.x(); y = a.y(); z = a.z(); return *this; }
};

tVector Vec(float x, float y, float z);
tVector Vec(btVector3 v);
#ifdef TMS_USE_LIB_3DS
tVector Vec3ds(Lib3dsVector v);
#endif
#ifdef TMS_USE_LIB_G3D
tVector VecG3D(G3DVector v[3]);
#endif
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
tVector Vec(const tVector2 &v, float y);
tVector Rotate(const tVector &vec, const tVector &axis, float angle);
double Det(const tVector &v1, const tVector &v2, const tVector &v3);
tVector Mix(const tVector v1, const tVector v2, float mix);
tVector Mix(const tVector v[], const float a[], int count, float mix);

#define vecmin(a, b) Vec(a.x > b.x ? b.x : a.x, a.y > b.y ? b.y : a.y, a.z > b.z ? b.z : a.z) 
#define vecmax(a, b) Vec(a.x < b.x ? b.x : a.x, a.y < b.y ? b.y : a.y, a.z < b.z ? b.z : a.z) 

int IsInside(tVector2 c[4], const tVector2 &p);
void DistToPlane(const tVector &a, const tVector &b, const tVector &c, const tVector &point, double *dist, tVector *dir);
void DistToPlane(const tVector &a, const tVector &b, const tVector &c, const tVector &d, const tVector &point, double *min_dist, tVector *min_dir);

tVector PointToLine(tVector p, tVector r, tVector n);
float DistToLine(tVector p, tVector r, tVector n);

int PointInRange(tVector p, tVector s, tVector b);

tVector RandVec(void);

#endif
