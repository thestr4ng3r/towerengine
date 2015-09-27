
#include <towerengine.h>


const float tMatrix4::identity_matrix[16] = { 1.0, 0.0, 0.0, 0.0,
										  	  0.0, 1.0, 0.0, 0.0,
										  	  0.0, 0.0, 1.0, 0.0,
										  	  0.0, 0.0, 0.0, 1.0 };




inline void MultiplyMatrix4(float a[16], float b[16], float *r)
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

inline bool InverseMatrix4(const float m[16], float invOut[16])
{
	float inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
              m[4]  * m[11] * m[14] +
              m[8]  * m[6]  * m[15] -
              m[8]  * m[7]  * m[14] -
              m[12] * m[6]  * m[11] +
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] -
               m[8]  * m[6] * m[13] -
               m[12] * m[5] * m[10] +
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
              m[1]  * m[11] * m[14] +
              m[9]  * m[2] * m[15] -
              m[9]  * m[3] * m[14] -
              m[13] * m[2] * m[11] +
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
              m[0]  * m[11] * m[13] +
              m[8]  * m[1] * m[15] -
              m[8]  * m[3] * m[13] -
              m[12] * m[1] * m[11] +
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
              m[0]  * m[7] * m[14] +
              m[4]  * m[2] * m[15] -
              m[4]  * m[3] * m[14] -
              m[12] * m[2] * m[7] +
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
               m[0]  * m[6] * m[13] +
               m[4]  * m[1] * m[14] -
               m[4]  * m[2] * m[13] -
               m[12] * m[1] * m[6] +
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[7] +
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[9] -
               m[8] * m[1] * m[7] +
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for(i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
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

void tMatrix4::SetInverse(void)
{
	float temp[16];
	InverseMatrix4(v, temp);
	memcpy(v, temp, sizeof(float) * 16);
}

void tMatrix4::GetDataTranspose(float *out) const
{
	out[0] = v[0];
	out[1] = v[4];
	out[2] = v[8];
	out[3] = v[12];
	out[4] = v[1];
	out[5] = v[5];
	out[6] = v[9];
	out[7] = v[13];
	out[8] = v[2];
	out[9] = v[6];
	out[10] = v[10];
	out[11] = v[14];
	out[12] = v[3];
	out[13] = v[7];
	out[14] = v[11];
	out[15] = v[15];
}

tMatrix4 tMatrix4::GetInverse(void)
{
	tMatrix4 r;
	InverseMatrix4(v, r.GetData());
	return r;
}


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
