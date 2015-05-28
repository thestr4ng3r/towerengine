
#include "towerengine.h"

using namespace std;

tMaterialManager::tMaterialManager(void)
{
}

tMaterialManager::~tMaterialManager(void)
{
	map<string, tMaterial *>::iterator i;

	for(i=materials.begin(); i!=materials.end(); i++)
		delete i->second;
}

tMaterial *tMaterialManager::GetMaterial(string name)
{
	map<string, tMaterial *>::iterator i = materials.find(name);

	if(i == materials.end())
		return 0;

	return i->second;
}

void tMaterialManager::AddMaterial(string name, tMaterial *material)
{
	map<string, tMaterial *>::iterator i = materials.find(name);

	if(i != materials.end())
		throw "material name already taken.";

	materials[name] = material;
}
