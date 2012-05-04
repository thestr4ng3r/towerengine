#ifndef _CVECTOR_H
#define _CVECTOR_H

#define pvec(v) v.x, v.y, v.z
#define pvec2(v) v.x, v.y

struct CVector
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

	CVector2 xy(void) { return Vec(x, y); };
	CVector2 xz(void) { return Vec(x, z); };
	CVector2 yx(void) { return Vec(y, x); };
	CVector2 yz(void) { return Vec(y, z); };
	CVector2 zx(void) { return Vec(z, x); };
	CVector2 zy(void) { return Vec(z, y); };

	void Set(float _x, float _y, float _z) { x = _x; y = _y; z = _z; };
	float Len(void) const		{ return sqrt(x * x + y * y + z * z); };
   float SquaredLen(void) const		{ return x * x + y * y + z * z; };
	void Normalize(void)		{ float l; l = 1.0 / Len(); x *= l; y *= l; z *= l; };
	void PutToGL(void) const	{ glVertex3fv(v); };
	void NormalToGL(void) const	{ glNormal3fv(v); };
	void TexcoordToGL(void) const	{ glMultiTexCoord3fvARB(GL_TEXTURE0_ARB, v); };
	void TangXToGL(void) const	{ glMultiTexCoord3fvARB(GL_TEXTURE1_ARB, v); };
	void TangYToGL(void) const	{ glMultiTexCoord3fvARB(GL_TEXTURE2_ARB, v); };
	void AttrToGL(int which) const             { glVertexAttrib3fv(which, v); };
	void SetFromPlane(const CVector2 &p, const CVector &n, float d);
	CVector operator -(void) const	{CVector r; r.x = -x, r.y = -y, r.z = -z; return r; };
	operator CVector2() const	{CVector2 r; r.x = x; r.y = z; return r; };

	CVector &operator+=(const CVector o)	{x += o.x; y += o.y; z += o.z; return *this;};
	CVector &operator-=(const CVector o)	{x -= o.x; y -= o.y; z -= o.z; return *this;};
	CVector &operator*=(const CVector o)	{x *= o.x; y *= o.y; z *= o.z; return *this;};
	CVector &operator*=(float f)		{x *= f; y *= f; z *= f; return *this;};
	CVector &operator/=(float f)		{x /= f; y /= f; z /= f; return *this;};
	#ifdef TMS_USE_LIB_3DS
        CVector &operator=(Lib3dsVector o)      {x = o[0]; y = o[1]; z = o[2]; return *this;};
	#endif
        /*#ifdef TMS_USE_LIB_3DS
        CVector &operator=(G3DVector o[3])      {x = o[0]; y = o[1]; z = o[2]; return *this;};
        #endif*/
};

CVector Vec(float x, float y, float z);
#ifdef TMS_USE_LIB_3DS
CVector Vec3ds(Lib3dsVector v);
#endif
#ifdef TMS_USE_LIB_G3D
CVector VecG3D(G3DVector v[3]);
#endif
CVector NullVec(void);
CVector operator+(const CVector &a, const CVector &b);
CVector operator-(const CVector &a, const CVector &b);
CVector operator*(const CVector &a, float scalar);
CVector operator*(float scalar, const CVector &a);
CVector operator*(const CVector &a, const CVector &b);
int operator==(const CVector a, const CVector b);
double Dot(const CVector &a, const CVector &b);
CVector Cross(const CVector &a, const CVector &b);
CVector Vec(const CVector2 &v, float y);
CVector Rotate(const CVector &vec, const CVector &axis, float angle);
double Det(const CVector &v1, const CVector &v2, const CVector &v3);
CVector Mix(const CVector v1, const CVector v2, float mix);
CVector Mix(const CVector v[], const float a[], int count, float mix);

#define vecmin(a, b) Vec(a.x > b.x ? b.x : a.x, a.y > b.y ? b.y : a.y, a.z > b.z ? b.z : a.z) 
#define vecmax(a, b) Vec(a.x < b.x ? b.x : a.x, a.y < b.y ? b.y : a.y, a.z < b.z ? b.z : a.z) 

int IsInside(CVector2 c[4], const CVector2 &p);
void DistToPlane(const CVector &a, const CVector &b, const CVector &c, const CVector &point, double *dist, CVector *dir);
void DistToPlane(const CVector &a, const CVector &b, const CVector &c, const CVector &d, const CVector &point, double *min_dist, CVector *min_dir);

CVector PointToLine(CVector p, CVector r, CVector n);
float DistToLine(CVector p, CVector r, CVector n);

int PointInRange(CVector p, CVector s, CVector b);

CVector RandVec(void);

#endif
