
#ifndef _MATRIX4_H
#define _MATRIX4_H


class tMatrix4
{
	private:
		float v[16];

	public:
		static const float identity_matrix[16];

		tMatrix4(float v[16]);
		tMatrix4(void);

		void Set(float v[16]);

		float *GetData(void)	{ return v; }

		void SetIdentity(void);

		static tMatrix4 GetIdentity(void)		{ return tMatrix4(); }
};

tMatrix4 operator*(tMatrix4 a, tMatrix4 b);

void CombineMatrix4(float MatrixA[16],float MatrixB[16], float *retM);
void InverseMatrix4(float m[16], float *ret);
tVector ApplyMatrix4(float m[16], tVector v);
void PrintMatrix4(const float m[16]);

#endif
