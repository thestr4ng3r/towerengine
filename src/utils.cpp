#include "towerengine.h"

#include <fstream>

#ifdef _MSC_VER
#define snprintf sprintf_s
#endif

using namespace std;

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
	snprintf(r, strlen(str) + 1, "%s", str);
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


string PathOfFile(string file)
{
    int i;

    for(i = file.length()-1; i > 0; i--)
    {
		char c = file.at(i);

		if(c == '/' || c == '\\')
			break;
    }

	return file.substr(0, i+1);
}



void PrintMatrix4(const float m[16])
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


char *ReadFile(const char *file, size_t &size)
{
	ifstream in;
	in.open(file, ios::in | ios::binary | ios::ate);

	if(in.is_open())
	{
		in.seekg(0, ios::end);
		size = (size_t)in.tellg();
		in.seekg(0, ios::beg);

		char *data = new char[size];
		in.read(data, size);
		return data;
	}

	return 0;
}


char *ReadFile(const char *file)
{
	size_t size;
	ifstream in;
	in.open(file, ios::in | ios::binary | ios::ate);

	if(in.is_open())
	{
		in.seekg(0, ios::end);
		size = (size_t)in.tellg();
		in.seekg(0, ios::beg);

		char *data = new char[size + 1];
		in.read(data, size);
		data[size] = 0;
		return data;
	}

	return 0;
}
















