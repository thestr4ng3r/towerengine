
#ifndef _MATRIX3_H
#define _MATRIX3_H


class tMatrix3
{
	private:
		tVector x;
		tVector y;
		tVector z;

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
		static tMatrix3 FromBtMatrix(const btMatrix3x3 &matrix);
};

inline tVector operator*(tVector v, tMatrix3 m)		{ return m.ApplyToVector(v); }
inline tVector operator*(tMatrix3 m, tVector v)		{ return m.ApplyToVector(v); }
tMatrix3 operator*(tMatrix3 a, tMatrix3 b);


#endif
