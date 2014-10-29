#include "towerengine.h"

char *ftoa(float f, int len)
{
	char *buf = new char[len + 1];
	snprintf(buf, len + 1, "%f", f);
	int i;
	for(i=0; i<(int)strlen(buf); i++)
		buf[i] = buf[i] == ',' ? '.' : buf[i];
	return buf;
}

char *itoa(int v, int len)
{
	char *buf = new char[len + 1];
	snprintf(buf, len + 1, "%d", v);
	return buf;
}

char *cstr(const char *str)
{
	char *r;
	r = new char[strlen(str) + 1];
	snprintf(r, strlen(str)+1, "%s", str);
	return r;
}

void SetColor(GLenum which, float r, float g, float b)
{
	float c[4];

	c[0] = r;
	c[1] = g;
	c[2] = b;
	c[3] = 0.0;
	glColor3f(r, g, b);
	glMaterialfv(GL_FRONT_AND_BACK, which, c);
}

void PaintAxes(void)
{
	glDisable(GL_LIGHTING);

	glLineWidth(3);

	SetColor(GL_DIFFUSE, 1.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, -1.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glEnd();

	SetColor(GL_DIFFUSE, 1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(-1.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);
	glEnd();

	SetColor(GL_DIFFUSE, 0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, -1.0);
	glVertex3f(0.0, 0.0, 1.0);
	glEnd();

	SetColor(GL_DIFFUSE, 1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	glVertex3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 1.0);
	glEnd();
}


char *PathOfFile(const char *file)
{
    int i;
    char *path = 0;

    if(!file)
    {
        path = new char[0];
        return path;
    }

    for(i = strlen(file); i > 0; i--)
    {
        path = new char [i];
        snprintf(path, i, "%s", file);
        if(path[strlen(path) - 1] == '/' || path[strlen(path) - 1] == '\\')
            break;
    }

    return path;
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

CVector ApplyMatrix4(float m[16], CVector v)
{
	CVector r;
	r.x = Dot(Vec(m[0], m[1], m[2]), v) + m[3];
	r.y = Dot(Vec(m[4], m[5], m[6]), v) + m[7];
	r.z = Dot(Vec(m[8], m[9], m[10]), v) + m[11];

	return r;
}

void PrintMatrix4(float m[16])
{
	printf("%f, %f, %f, %f,\n%f, %f, %f, %f,\n%f, %f, %f, %f,\n%f, %f, %f, %f\n",
				m[0], m[1], m[2], m[3],
				m[4], m[5], m[6], m[7],
				m[8], m[9], m[10], m[11],
				m[12], m[13], m[14], m[15]);
}

#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

static const unsigned char d[] = {
    66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

int Base64Decode(char *in, size_t inLen, unsigned char *out, size_t *outLen)
{
    char *end = in + inLen;
    size_t buf = 1, len = 0;

    while (in < end) {
        unsigned char c = d[(unsigned int)*in++];

        switch (c) {
        case WHITESPACE: continue;   /* skip whitespace */
        case INVALID:    return 1;   /* invalid input, return error */
        case EQUALS:                 /* pad character, end of data */
            in = end;
            continue;
        default:
            buf = buf << 6 | c;

            /* If the buffer is full, split it into bytes */
            if (buf & 0x1000000) {
                if ((len += 3) > *outLen) return 1; /* buffer overflow */
                *out++ = buf >> 16;
                *out++ = buf >> 8;
                *out++ = buf;
                buf = 1;
            }
        }
    }

    if (buf & 0x40000) {
        if ((len += 2) > *outLen) return 1; /* buffer overflow */
        *out++ = buf >> 10;
        *out++ = buf >> 2;
    }
    else if (buf & 0x1000) {
        if (++len > *outLen) return 1; /* buffer overflow */
        *out++ = buf >> 4;
    }

    *outLen = len; /* modify to reflect the actual output size */
    return 0;
}

void Base64Decode(char *in, unsigned char **out, size_t *out_size)
{
	size_t in_size = strlen(in);
	*out_size = (size_t)ceil((float)in_size * 1.4);
	*out = new unsigned char[*out_size];
	Base64Decode(in, in_size, *out, out_size);
}

float RandomFloat(float min, float max)
{
	return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

float Mix(float a, float b, float mix)
{
	return a + (b-a) * max(min(mix, 1.0f), 0.0f);
}


xmlNodePtr WriteDataXMLNode(const char *data, unsigned int size, xmlDocPtr doc)
{
	return 0;
}




















