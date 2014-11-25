
#ifndef _TRANSFORM_H
#define _TRANSFORM_H


class tMatrix3
{
	private:
		tVector x, y, z;

	public:
		tMatrix3(tVector x, tVector y, tVector z);
		tMatrix3(void);

		void Set(tVector x, tVector y, tVector z);

		tVector GetX(void)		{ return x; }
		tVector GetY(void)		{ return y; }
		tVector GetZ(void)		{ return z; }

		void SetX(tVector x)	{ this->x = x; }
		void SetY(tVector y)	{ this->y = y; }
		void SetZ(tVector z)	{ this->z = z; }

		void SetIdentity(void);
		void SetEuler(tVector rot);
		void Scale(tVector scale);

		tVector ApplyToVector(tVector v);

		btMatrix3x3 ToBtMatrix(void);

		static tMatrix3 GetIdentity(void)		{ return tMatrix3(); }
		static tMatrix3 GetEuler(tVector rot)	{ tMatrix3 r; r.SetEuler(rot); return r; }
		static tMatrix3 FromBtMatrix(btMatrix3x3 matrix);
};

inline tVector operator*(tVector v, tMatrix3 m)		{ return m.ApplyToVector(v); }
inline tVector operator*(tMatrix3 m, tVector v)		{ return m.ApplyToVector(v); }
tMatrix3 operator*(tMatrix3 a, tMatrix3 b);

class tTransform
{
	private:
		tMatrix3 basis;
		tVector position;

	public:
		tTransform(tMatrix3 basis, tVector position);
		tTransform(void);

		tMatrix3 GetBasis(void)				{ return basis; }
		tVector GetPosition(void)			{ return position; }

		float *GetMatrix(float *matrix);

		void Set(tMatrix3 basis, tVector position);

		void SetBasis(tMatrix3 basis)		{ this->basis = basis; }
		void SetPosition(tVector position)	{ this->position = position; }

		void TranslateLocal(tVector v)		{ position += v * basis; }
		void TranslateWorld(tVector v)		{ position += v; }

		tVector ApplyToVector(tVector v);

		void SetIdentity(void);

		btTransform ToBtTransform(void);

		static tTransform GetIdentity(void)	{ return tTransform(); }
		static tTransform FromBtTransform(btTransform trans);
};

inline tVector operator*(tVector v, tTransform t)		{ return t.ApplyToVector(v); }
inline tVector operator*(tTransform t, tVector v)		{ return t.ApplyToVector(v); }


#endif
