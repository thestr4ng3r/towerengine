
#include "towerengine.h"


void CTransformationMatrix::LoadIdentity(void)
{
	memcpy(matrix, CEngine::identity_matrix4, 16 * sizeof(float));
}

void CTransformationMatrix::Translate(CVector v)
{
	float m[16] =
		{ 1.0, 0.0, 0.0, v.x,
		  0.0, 1.0, 0.0, v.y,
		  0.0, 0.0, 1.0, v.z,
		  0.0, 0.0, 0.0, 1.0 };

	CombineMatrix4(matrix, m, matrix);
}

void CTransformationMatrix::RotateX(float a)
{
	float m[16] =
		{ (float)cos(a), -(float)sin(a), 0.0, 0.0,
		  (float)sin(a), (float)cos(a),  0.0, 0.0,
		  0.0,    0.0,     1.0, 0.0,
		  0.0,    0.0,     0.0, 1.0 };
	CombineMatrix4(matrix, m, matrix);
}

void CTransformationMatrix::RotateY(float a)
{
	float m[16] =
		{ (float)cos(a), 0.0,  (float)sin(a), 0.0,
		  0.0,    1.0,     0.0, 0.0,
		  -(float)sin(a),0.0,  (float)cos(a), 0.0,
		  0.0,    0.0,     0.0, 1.0 };
	CombineMatrix4(matrix, m, matrix);
}

void CTransformationMatrix::RotateZ(float a)
{
	float m[16] =
		{ 1.0,    0.0,     0.0, 0.0,
		  0.0, (float)cos(a), -(float)sin(a), 0.0,
		  0.0, (float)sin(a),  (float)cos(a), 0.0,
		  0.0,    0.0,     0.0, 1.0 };
	CombineMatrix4(matrix, m, matrix);
}

void CTransformationMatrix::Rotate(CVector v)
{
	RotateX(v.x);
	RotateY(v.y);
	RotateZ(v.z);
}

void CTransformationMatrix::Scale(CVector v)
{
	matrix[0] *= v.x;
	matrix[1] *= v.x;
	matrix[3] *= v.x;
	matrix[5] *= v.y;
	matrix[6] *= v.y;
	matrix[7] *= v.y;
	matrix[9] *= v.z;
	matrix[10] *= v.z;
	matrix[11] *= v.z;
}

void CTransformationMatrix::SetXY(CVector x, CVector y)
{
	CVector z = Cross(x, y);
	SetXYZ(x, y, z);
}

void CTransformationMatrix::SetYZ(CVector y, CVector z)
{
	CVector x = Cross(y, z);
	SetXYZ(x, y, z);
}

void CTransformationMatrix::SetXZ(CVector x, CVector z)
{
	CVector y = Cross(x, z);
	SetXYZ(x, y, z);
}

void CTransformationMatrix::SetXYZ(CVector x, CVector y, CVector z)
{
	/*float m[16] =
			{ x.x, y.x, z.x, 0.0,
			  x.y, y.y, z.y, 0.0,
			  x.z, y.z, z.z, 0.0,
			  0.0, 0.0, 0.0, 1.0 };*/

	float m[16] =
				{ x.x, x.y, x.z, 0.0,
				  y.x, y.y, y.z, 0.0,
				  z.x, z.y, z.z, 0.0,
				  0.0, 0.0, 0.0, 1.0 };

	CombineMatrix4(matrix, m, matrix);
}
