
#include <towerengine.h>


const float tMatrix4::identity_matrix[16] = { 1.0, 0.0, 0.0, 0.0,
										  	  0.0, 1.0, 0.0, 0.0,
										  	  0.0, 0.0, 1.0, 0.0,
										  	  0.0, 0.0, 0.0, 1.0 };




void MultiplyMatrix4(float a[16], float b[16], float *r)
{
	int i;

	for(i = 0; i < 4; i++)
	{
		r[i*4] = a[i*4] * b[0] + a[i*4+1] * b[4] + a[i*4+2] * b[8] + a[i*4+3] * b[12];
		r[i*4+1] = a[i*4] * b[1] + a[i*4+1] * b[5] + a[i*4+2] * b[9] + a[i*4+3] * b[13];
		r[i*4+2] = a[i*4] * b[2] + a[i*4+1] * b[6] + a[i*4+2] * b[10] + a[i*4+3] * b[14];
		r[i*4+3] = a[i*4] * b[3] + a[i*4+1] * b[7] + a[i*4+2] * b[11] + a[i*4+3] * b[15];
	}
}

void InverseMatrix4(float m[16], float *r)
{
	r[0] = m[0];
	r[1] = m[4];
	r[2] = m[8];
	r[4] = m[1];
	r[5] = m[5];
	r[6] = m[9];
	r[8] = m[2];
	r[9] = m[6];
	r[10] = m[10];

	r[12] = r[0]*-m[12]+r[4]*-m[13]+r[8]*-m[14];
	r[13] = r[1]*-m[12]+r[5]*-m[13]+r[9]*-m[14];
	r[14] = r[2]*-m[12]+r[6]*-m[13]+r[10]*-m[14];

	r[3] = 0.0f;
	r[7] = 0.0f;
	r[11] = 0.0f;
	r[15] = 1.0f;
}

tMatrix4::tMatrix4(float v[16])
{
	Set(v);
}

tMatrix4::tMatrix4(void)
{
	SetIdentity();
}

void tMatrix4::Set(float v[16])
{
	memcpy(this->v, v, sizeof(float) * 16);
}

void tMatrix4::SetIdentity(void)
{
	memcpy(this->v, identity_matrix, sizeof(float) * 16);
}

void tMatrix4::SetTranslation(tVector t)
{
	SetIdentity();

	v[3] = t.x;
	v[7] = t.y;
	v[11] = t.z;
}

void tMatrix4::SetLookAt(tVector eye, tVector center, tVector up)
{
	tVector f = center - eye;
	f.Normalize();

	up.Normalize();

	tVector s = Cross(f, up);
	s.Normalize();

	up = Cross(s, f);

	f *= -1.0;

	memcpy(v+0, s.v, sizeof(float) * 3);	v[3] = 0.0;
	memcpy(v+4, up.v, sizeof(float) * 3);	v[7] = 0.0;
	memcpy(v+8, f.v, sizeof(float) * 3);	v[11] = 0.0;
	v[12] = 0.0; v[13] = 0.0; v[14] = 0.0; 	v[15] = 1.0;

	tMatrix4 trans;
	trans.SetTranslation(-eye);
	SetMultiply(trans);
}

void tMatrix4::SetOrtho(float left, float right, float top, float bottom, float near_clip, float far_clip)
{
	SetIdentity();

	v[0] = 2.0 / (right - left);				v[3] = -((right + left) / (right - left));
	v[5] = 2.0 / (top - bottom);				v[7] = -((top + bottom) / (top - bottom));
	v[10] = -2.0 / (far_clip - near_clip);		v[11] = -((far_clip + near_clip) / (far_clip - near_clip));
	//v[15] = 1.0;
}

void tMatrix4::SetPerspective(float fovy, float aspect, float near_clip, float far_clip)
{
	SetIdentity();

	float f = 1.0 / tan(degtorad(fovy) / 2.0);

	v[0] = f / aspect;
	v[5] = f;
	v[10] = (far_clip + near_clip) / (near_clip - far_clip);	v[11] = (2 * far_clip * near_clip) / (near_clip - far_clip);
	v[14] = -1.0;
	v[15] = 0.0;
}


void tMatrix4::SetMultiply(tMatrix4 m)
{
	float temp[16];
	MultiplyMatrix4(v, m.GetData(), temp);
	memcpy(v, temp, sizeof(float) * 16);
}

/*void tMatrix4::GLMultMatrix(void)
{
	glMultTransposeMatrixf(v);
}*/


tMatrix4 operator*(tMatrix4 &a, tMatrix4 &b)
{
	tMatrix4 r;
	MultiplyMatrix4(a.GetData(), b.GetData(), r.GetData());
	return r;
}



tVector ApplyMatrix4(float m[16], tVector v)
{
	tVector r;
	r.x = Dot(Vec(m[0], m[1], m[2]), v) + m[3];
	r.y = Dot(Vec(m[4], m[5], m[6]), v) + m[7];
	r.z = Dot(Vec(m[8], m[9], m[10]), v) + m[11];

	return r;
}
