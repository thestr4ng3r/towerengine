
#include "towerengine.h"


tTransform::tTransform(tMatrix3 basis, tVector position)
{
	this->basis = basis;
	this->position = position;
}

tTransform::tTransform(void)
{
	SetIdentity();
}

float *tTransform::GetMatrix(float *matrix)
{
	memcpy(matrix, basis.x.v, sizeof(float)*3);
	memcpy(matrix+4, basis.y.v, sizeof(float)*3);
	memcpy(matrix+8, basis.z.v, sizeof(float)*3);
	matrix[3] = position.x; matrix[7] = position.y; matrix[11] = position.z;
	//matrix[0] = basis.GetX().x; matrix[1] = basis.GetY().x; matrix[2] = basis.GetZ().x;		matrix[3] = position.x;
	//matrix[4] = basis.GetX().y; matrix[5] = basis.GetY().y; matrix[6] = basis.GetZ().y;		matrix[7] = position.y;
	//matrix[8] = basis.GetX().z; matrix[9] = basis.GetY().z; matrix[10] = basis.GetZ().z;	matrix[11] = position.z;
	matrix[12] = 0.0; matrix[13] = 0.0; matrix[14] = 0.0; matrix[15] = 1.0;

	return matrix;
}

void tTransform::Set(tMatrix3 basis, tVector position)
{
	this->basis = basis;
	this->position = position;
}

tVector tTransform::ApplyToVector(tVector v)
{
	return basis.ApplyToVector(v) + position;
}

void tTransform::SetIdentity(void)
{
	basis = tMatrix3::GetIdentity();
	position = Vec(0.0, 0.0, 0.0);
}

tTransform tTransform::FromBtTransform(const btTransform &trans)
{
	return tTransform(tMatrix3::FromBtMatrix(trans.getBasis()), Vec(trans.getOrigin()));
}



