
#include "towerengine.h"

tMeshMaterial::tMeshMaterial(tMesh *mesh, string name) : tMaterial()
{
	ibo_data = 0;
	triangle_count = 0;
	triangle_i = 0;

    this->mesh = mesh;
    this->name = name;
	mesh->AddMaterial(this);
	ibo = mesh->CreateIBO();
}

tMeshMaterial::~tMeshMaterial(void)
{
	if(mesh)
		mesh->RemoveMaterial(this);
}
