%module towerengine
%{
#define SWIG_FILE_WITH_INIT
#include "includes.h"
#include "towerengine.h"
%}

typedef unsigned int GLuint;

#include "vector2_swig.h"
#include "vector_swig.h"
#include "matrix3_swig.h"

#include "_towerengine.h"