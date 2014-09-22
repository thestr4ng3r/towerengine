#ifndef _TOWERENGINE_UTILS_H
#define _TOWERENGINE_UTILS_H

#define degtorad(x) (x * M_PI / 180.0)

char *ftoa(float f, int len = 50);
char *itoa(int v, int len = 50);
char *cstr(const char *);

void SetColor(GLenum which, float r, float g, float b);
char *PathOfFile(const char *file);
void PaintAxes(void);

void CombineMatrix4(float MatrixA[16],float MatrixB[16], float *retM);
void InverseMatrix4(float m[16], float *ret);
CVector ApplyMatrix4(float m[16], CVector v);
void PrintMatrix4(float m[16]);

int Base64Decode(char *in, size_t inLen, unsigned char *out, size_t *outLen);
void Base64Decode(char *in, unsigned char **out, size_t *out_size);

float RandomFloat(float min = 0.0, float max = 1.0);
float Mix(float a, float b, float mix);

#endif
