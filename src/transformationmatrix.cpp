
#include "towerengine.h"


void tTransformationMatrix::LoadIdentity(void)
{
	memcpy(matrix, tEngine::identity_matrix4, 16 * sizeof(float));
}

void tTransformationMatrix::Translate(tVector v)
{
	float m[16] =
		{ 1.0, 0.0, 0.0, v.x,
		  0.0, 1.0, 0.0, v.y,
		  0.0, 0.0, 1.0, v.z,
		  0.0, 0.0, 0.0, 1.0 };

	CombineMatrix4(matrix, m, matrix);
}

void tTransformationMatrix::RotateX(float a)
{
	float m[16] =
		{ (float)cos(a), -(float)sin(a), 0.0, 0.0,
		  (float)sin(a), (float)cos(a),  0.0, 0.0,
		  0.0,    0.0,     1.0, 0.0,
		  0.0,    0.0,     0.0, 1.0 };
	CombineMatrix4(matrix, m, matrix);
}

void tTransformationMatrix::RotateY(float a)
{
	float m[16] =
		{ (float)cos(a), 0.0,  (float)sin(a), 0.0,
		  0.0,    1.0,     0.0, 0.0,
		  -(float)sin(a),0.0,  (float)cos(a), 0.0,
		  0.0,    0.0,     0.0, 1.0 };
	CombineMatrix4(matrix, m, matrix);
}

void tTransformationMatrix::RotateZ(float a)
{
	float m[16] =
		{ 1.0,    0.0,     0.0, 0.0,
		  0.0, (float)cos(a), -(float)sin(a), 0.0,
		  0.0, (float)sin(a),  (float)cos(a), 0.0,
		  0.0,    0.0,     0.0, 1.0 };
	CombineMatrix4(matrix, m, matrix);
}

void tTransformationMatrix::Rotate(tVector v)
{
	RotateX(v.x);
	RotateY(v.y);
	RotateZ(v.z);
}

void tTransformationMatrix::Scale(tVector v)
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

void tTransformationMatrix::SetXY(tVector x, tVector y)
{
	tVector z = Cross(x, y);
	SetXYZ(x, y, z);
}

void tTransformationMatrix::SetYZ(tVector y, tVector z)
{
	tVector x = Cross(y, z);
	SetXYZ(x, y, z);
}

void tTransformationMatrix::SetXZ(tVector x, tVector z)
{
	tVector y = Cross(x, z);
	SetXYZ(x, y, z);
}

void tTransformationMatrix::SetXYZ(tVector x, tVector y, tVector z)
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
