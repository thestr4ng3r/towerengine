
#ifndef _MATRIX3_H
#define _MATRIX3_H


struct tMatrix3
{
	tVector x;
	tVector y;
	tVector z;

	tMatrix3(tVector x, tVector y, tVector z);
	tMatrix3(void);

	void Set(tVector x, tVector y, tVector z);

	void SetIdentity(void);
	void SetEuler(tVector rot);
	void Scale(tVector scale);

	tVector ApplyToVector(tVector v);

	btMatrix3x3 ToBtMatrix(void);
	btMatrix3x3 &ToBtMatrix(btMatrix3x3 &bt_matrix);

	static tMatrix3 GetIdentity(void)		{ return tMatrix3(); }
	static tMatrix3 GetEuler(tVector rot)	{ tMatrix3 r; r.SetEuler(rot); return r; }
	static tMatrix3 FromBtMatrix(const btMatrix3x3 &matrix);
};

inline tVector operator*(tVector v, tMatrix3 m)		{ return m.ApplyToVector(v); }
inline tVector operator*(tMatrix3 m, tVector v)		{ return m.ApplyToVector(v); }
tMatrix3 operator*(tMatrix3 a, tMatrix3 b);


#endif
