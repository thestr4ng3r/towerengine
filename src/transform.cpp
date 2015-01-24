
#include "towerengine.h"

tMatrix3::tMatrix3(tVector x, tVector y, tVector z)
{
	Set(x, y, z);
}

tMatrix3::tMatrix3(void)
{
	SetIdentity();
}

void tMatrix3::Set(tVector x, tVector y, tVector z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void tMatrix3::SetIdentity(void)
{
	x = Vec(1.0, 0.0, 0.0);
	y = Vec(0.0, 1.0, 0.0);
	z = Vec(0.0, 0.0, 1.0);
}

void tMatrix3::SetEuler(tVector rot)
{
	tVector s = Vec(sin(rot.x), sin(rot.y), sin(rot.z));
	tVector c = Vec(cos(rot.x), cos(rot.y), cos(rot.z));

	x = Vec(c.z * c.y - s.z * s.x * s.y,		-s.z * c.x, 		c.z * s.y + s.z * s.x * c.y);
	y = Vec(s.z * c.y + c.z * s.x * s.y,		c.z * c.x,			s.z * s.y - c.z * s.x * c.y);
	z = Vec(-c.x * s.y,							s.x,				c.x * c.y);
}

void tMatrix3::Scale(tVector scale)
{
	x *= scale.x;
	y *= scale.y;
	z *= scale.z;
}

tVector tMatrix3::ApplyToVector(tVector v)
{
	return v.x * Vec(x.x, y.x, z.x) + v.y * Vec(x.y, y.y, z.y) + v.z * Vec(x.z, y.z, z.z);
	//return x * v.x + y * v.y + z * v.z;
}

btMatrix3x3 tMatrix3::ToBtMatrix(void)
{
	return btMatrix3x3(	x.x, x.y, x.z,
						y.x, y.y, y.z,
						z.x, z.y, z.z);

	//return btMatrix3x3(	x.x, y.x, z.x,
	//					x.y, y.y, z.y,
	//					x.z, y.z, z.z);
}

tMatrix3 tMatrix3::FromBtMatrix(const btMatrix3x3 &matrix)
{
	return tMatrix3(Vec(matrix.getRow(0)), Vec(matrix.getRow(1)), Vec(matrix.getRow(2)));
}

tMatrix3 operator*(tMatrix3 a, tMatrix3 b)
{
	tMatrix3 r;
	tVector ax, ay, az, bx, by, bz;
	ax = a.GetX(); ay = a.GetY(); az = a.GetZ();
	bx = b.GetX(); by = b.GetY(); bz = b.GetZ();

	r.SetX(Vec(ax.x*bx.x + ax.y*by.x + ax.z*bz.x,	ax.x*bx.y + ax.y*by.y + ax.z*bz.y,	ax.x*bx.z + ax.y*by.z + ax.z*bz.z));
	r.SetY(Vec(ay.x*bx.x + ay.y*by.x + ay.z*bz.x,	ay.x*bx.y + ay.y*by.y + ay.z*bz.y,	ay.x*bx.z + ay.y*by.z + ay.z*bz.z));
	r.SetZ(Vec(az.x*bx.x + az.y*by.x + az.z*bz.x,	az.x*bx.y + az.y*by.y + az.z*bz.y,	az.x*bx.z + az.y*by.z + az.z*bz.z));

	return r;
}


tTransform::tTransform(tMatrix3 basis, tVector position)
{
	Set(basis, position);
}

tTransform::tTransform(void)
{
	SetIdentity();
}

float *tTransform::GetMatrix(float *matrix)
{
	memcpy(matrix, basis.GetX().v, sizeof(float)*3);
	memcpy(matrix+4, basis.GetY().v, sizeof(float)*3);
	memcpy(matrix+8, basis.GetZ().v, sizeof(float)*3);
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

btTransform tTransform::ToBtTransform(void)
{
	return btTransform(basis.ToBtMatrix(), BtVec(position));
}


tTransform tTransform::FromBtTransform(const btTransform &trans)
{
	return tTransform(tMatrix3::FromBtMatrix(trans.getBasis()), Vec(trans.getOrigin()));
}



