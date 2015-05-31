
#include <towerengine.h>


const float tMatrix4::identity_matrix[16] = { 1.0, 0.0, 0.0, 0.0,
										  	  0.0, 1.0, 0.0, 0.0,
										  	  0.0, 0.0, 1.0, 0.0,
										  	  0.0, 0.0, 0.0, 1.0 };


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


tMatrix4 operator*(tMatrix4 a, tMatrix4 b)
{
	// TODO
	return a;
}




// Matrix Functions

void CombineMatrix4(float MatrixA[16],float MatrixB[16], float *retM)
{
	float NewMatrix[16];
	int i;

	for(i = 0; i < 4; i++)
	{ //Cycle through each vector of first matrix.
		NewMatrix[i*4] = MatrixA[i*4] * MatrixB[0] + MatrixA[i*4+1] * MatrixB[4] + MatrixA[i*4+2] * MatrixB[8] + MatrixA[i*4+3] * MatrixB[12];
		NewMatrix[i*4+1] = MatrixA[i*4] * MatrixB[1] + MatrixA[i*4+1] * MatrixB[5] + MatrixA[i*4+2] * MatrixB[9] + MatrixA[i*4+3] * MatrixB[13];
		NewMatrix[i*4+2] = MatrixA[i*4] * MatrixB[2] + MatrixA[i*4+1] * MatrixB[6] + MatrixA[i*4+2] * MatrixB[10] + MatrixA[i*4+3] * MatrixB[14];
		NewMatrix[i*4+3] = MatrixA[i*4] * MatrixB[3] + MatrixA[i*4+1] * MatrixB[7] + MatrixA[i*4+2] * MatrixB[11] + MatrixA[i*4+3] * MatrixB[15];
	}
	memcpy(retM,NewMatrix,64);
}

void InverseMatrix4(float m[16], float *ret)
{
	float inv[16]; // The inverse will go here

	inv[0] = m[0];
	inv[1] = m[4];
	inv[2] = m[8];
	inv[4] = m[1];
	inv[5] = m[5];
	inv[6] = m[9];
	inv[8] = m[2];
	inv[9] = m[6];
	inv[10] = m[10];

	inv[12] = inv[0]*-m[12]+inv[4]*-m[13]+inv[8]*-m[14];
	inv[13] = inv[1]*-m[12]+inv[5]*-m[13]+inv[9]*-m[14];
	inv[14] = inv[2]*-m[12]+inv[6]*-m[13]+inv[10]*-m[14];

	inv[3] = 0.0f;
	inv[7] = 0.0f;
	inv[11] = 0.0f;
	inv[15] = 1.0f;

	memcpy(ret,inv,64);
}

tVector ApplyMatrix4(float m[16], tVector v)
{
	tVector r;
	r.x = Dot(Vec(m[0], m[1], m[2]), v) + m[3];
	r.y = Dot(Vec(m[4], m[5], m[6]), v) + m[7];
	r.z = Dot(Vec(m[8], m[9], m[10]), v) + m[11];

	return r;
}
