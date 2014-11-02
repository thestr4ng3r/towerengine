
#include "towerengine.h"

tMeshAsset::tMeshAsset(tMesh *mesh)
{
	this->mesh = mesh;
}

tMeshAsset::~tMeshAsset(void)
{
	delete mesh;
}


tCubeMapAsset::tCubeMapAsset(GLuint cubemap)
{
	this->cubemap = cubemap;
}

tCubeMapAsset::~tCubeMapAsset(void)
{
	glDeleteTextures(1, &cubemap);
}
