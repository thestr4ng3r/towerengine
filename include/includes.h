#ifndef _INCLUDES_H
#define _INCLUDES_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdexcept>


using namespace std;

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#define MAX_LINE_LEN 300

#ifdef _WIN32
#include <GL/glew.h>
#include <GL/wglew.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#ifdef TMS_USE_LIB_3DS
#include <lib3ds/file.h>
#include <lib3ds/mesh.h>
#include <lib3ds/material.h>
#endif
#ifdef TMS_USE_LIB_G3D
#include <g3d/types.h>
#include <g3d/g3d.h>
#endif

#include <IL/il.h>
#include <IL/ilu.h>

#include <libxml/tree.h>


#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#endif
