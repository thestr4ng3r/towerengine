%module towerengine_python
%{
#include "towerengine.h"
%}

typedef unsigned int GLuint;

struct tVector
{
	float x;
	float y;
	float z;
};

inline tVector Vec(float x, float y, float z)
{
	tVector v;

	v.x = x;
	v.y = y;
	v.z = z;

	return v;
}


struct tVector2
{
	float x;
	float y;
};


#define _VECTOR_H
#define _VECTOR2_H

#include "_towerengine.h"