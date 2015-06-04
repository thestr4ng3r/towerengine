
#include "towerengine.h"

tMaterialIBO::tMaterialIBO(tMesh *mesh)
{
	ibo_data = 0;
	ibo = new tIBO();
}

tMaterialIBO::~tMaterialIBO(void)
{
	delete ibo;
	delete [] ibo_data;
}
