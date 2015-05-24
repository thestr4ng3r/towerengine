
#include "towerengine.h"

tMaterialIBO::tMaterialIBO(tMesh *mesh)
{
	ibo_data = 0;
	ibo = mesh->CreateIBO();
}

tMaterialIBO::~tMaterialIBO(void)
{
	delete ibo;
	delete [] ibo_data;
}
