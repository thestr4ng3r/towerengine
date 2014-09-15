
#include "towerengine.h"

CMeshMaterial::CMeshMaterial(CMesh *mesh, string name) : CMaterial()
{
	ibo_data = 0;
	triangle_count = 0;
	triangle_i = 0;

    this->mesh = mesh;
    this->name = name;
	mesh->AddMaterial(this);
	ibo = mesh->CreateIBO();
}

CMeshMaterial::~CMeshMaterial(void)
{
	if(mesh)
		mesh->RemoveMaterial(this);
}
