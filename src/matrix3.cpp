
#include <towerengine.h>



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
	x.Set(1.0, 0.0, 0.0);
	y.Set(0.0, 1.0, 0.0);
	z.Set(0.0, 0.0, 1.0);
}

void tMatrix3::SetEuler(tVector rot)
{
	tVector s = tVec(sin(rot.x), sin(rot.y), sin(rot.z));
	tVector c = tVec(cos(rot.x), cos(rot.y), cos(rot.z));

	x = tVec(c.z * c.y - s.z * s.x * s.y, -s.z * c.x, c.z * s.y + s.z * s.x * c.y);
	y = tVec(s.z * c.y + c.z * s.x * s.y, c.z * c.x, s.z * s.y - c.z * s.x * c.y);
	z = tVec(-c.x * s.y, s.x, c.x * c.y);
}

void tMatrix3::Scale(tVector scale)
{
	x *= scale.x;
	y *= scale.y;
	z *= scale.z;
}

tVector tMatrix3::ApplyToVector(tVector v) const
{
	return v.x * tVec(x.x, y.x, z.x) + v.y * tVec(x.y, y.y, z.y) + v.z * tVec(x.z, y.z, z.z);
	//return x * v.x + y * v.y + z * v.z;
}

btMatrix3x3 tMatrix3::ToBtMatrix(void)
{
	return btMatrix3x3(	x.x, x.y, x.z,
						y.x, y.y, y.z,
						z.x, z.y, z.z);
}

btMatrix3x3 &tMatrix3::ToBtMatrix(btMatrix3x3 &bt_matrix)
{
	bt_matrix.setValue(x.x, x.y, x.z,
					   y.x, y.y, y.z,
					   z.x, z.y, z.z);
	return bt_matrix;
}

tMatrix3 tMatrix3::FromBtMatrix(const btMatrix3x3 &matrix)
{
	return tMatrix3(tVec(matrix.getRow(0)), tVec(matrix.getRow(1)), tVec(matrix.getRow(2)));
}

tMatrix3 operator*(const tMatrix3 &a, const tMatrix3 &b)
{
	tMatrix3 r;

	r.x = tVec(a.x.x * b.x.x + a.x.y * b.y.x + a.x.z * b.z.x, a.x.x * b.x.y + a.x.y * b.y.y + a.x.z * b.z.y,
			   a.x.x * b.x.z + a.x.y * b.y.z + a.x.z * b.z.z);
	r.y = tVec(a.y.x * b.x.x + a.y.y * b.y.x + a.y.z * b.z.x, a.y.x * b.x.y + a.y.y * b.y.y + a.y.z * b.z.y,
			   a.y.x * b.x.z + a.y.y * b.y.z + a.y.z * b.z.z);
	r.z = tVec(a.z.x * b.x.x + a.z.y * b.y.x + a.z.z * b.z.x, a.z.x * b.x.y + a.z.y * b.y.y + a.z.z * b.z.y,
			   a.z.x * b.x.z + a.z.y * b.y.z + a.z.z * b.z.z);

	return r;
}
