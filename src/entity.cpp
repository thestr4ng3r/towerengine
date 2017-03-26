#include "towerengine.h"

using namespace std;

tEntity::tEntity(string name, string group)
{
	this->name = name;
	this->group = group;
}

tEntity::~tEntity()
{
}


void tEntity::SetAttribute(string name, tEntityAttribute attr)
{
	attributes[name] = attr;
}

tEntityAttribute *tEntity::GetAttribute(std::string name)
{
	map<string, tEntityAttribute>::iterator i = attributes.find(name);

	if(i == attributes.end())
		return 0;

	tEntityAttribute &attr = i->second;
	return &attr;
}

