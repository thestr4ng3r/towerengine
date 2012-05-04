#ifndef _FTOA_H
#define _FTOA_H

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

#endif
