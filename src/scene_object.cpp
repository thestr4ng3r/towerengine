
#include "towerengine.h"

using namespace std;



void tSceneObject::SetAttribute(string name, string value)
{
	attributes.insert(pair<string, string>(name, value));
}

string tSceneObject::GetAttribute(string name)
{
	map<string, string>::iterator i = attributes.find(name);

	if(i != attributes.end())
		return i->second;

	return string();
}

string tSceneObject::GetAttribute(string name, string default_v)
{
	map<string, string>::iterator i = attributes.find(name);

	if(i != attributes.end())
		return i->second;

	return default_v;
}

int tSceneObject::GetAttributeInt(string name, int default_v)
{
	map<string, string>::iterator i = attributes.find(name);

	if(i != attributes.end())
		return atoi(i->second.c_str());

	return default_v;
}

float tSceneObject::GetAttributeFloat(string name, float default_v)
{
	map<string, string>::iterator i = attributes.find(name);

	if(i != attributes.end())
		return atof(i->second.c_str());

	return default_v;
}

// ---------------------------------------------

tObjectSceneObject::tObjectSceneObject(tObject *object)
{
	this->object = object;
}

tObjectSceneObject::~tObjectSceneObject(void)
{
	delete object;
}

void tObjectSceneObject::AddToWorld(tWorld *world)
{
	world->AddObject(object);
}

void tObjectSceneObject::RemoveFromWorld(tWorld *world)
{
	world->RemoveObject(object);
}

// ---------------------------------------------

tDirectionalLightSceneObject::tDirectionalLightSceneObject(tDirectionalLight *light)
{
	this->light = light;
}

tDirectionalLightSceneObject::~tDirectionalLightSceneObject(void)
{
	delete light;
}

void tDirectionalLightSceneObject::AddToWorld(tWorld *world)
{
	world->AddDirectionalLight(light);
}

void tDirectionalLightSceneObject::RemoveFromWorld(tWorld *world)
{
	world->RemoveDirectionalLight(light);
}

// ---------------------------------------------

tPointLightSceneObject::tPointLightSceneObject(tPointLight *light)
{
	this->light = light;
}

tPointLightSceneObject::~tPointLightSceneObject(void)
{
	delete light;
}

void tPointLightSceneObject::AddToWorld(tWorld *world)
{
	world->AddPointLight(light);
}

void tPointLightSceneObject::RemoveFromWorld(tWorld *world)
{
	world->RemovePointLight(light);
}

// ---------------------------------------------

tEmptySceneObject::tEmptySceneObject(tTransform transform)
{
	this->transform = transform;
}

tEmptySceneObject::~tEmptySceneObject(void)
{
}

// ---------------------------------------------

tCubeMapReflectionSceneObject::tCubeMapReflectionSceneObject(tCubeMapReflection *reflection)
{
	this->reflection = reflection;
}

tCubeMapReflectionSceneObject::~tCubeMapReflectionSceneObject()
{
	delete reflection;
}

void tCubeMapReflectionSceneObject::AddToWorld(tWorld *world)
{
	world->AddCubeMapReflection(reflection);
}

void tCubeMapReflectionSceneObject::RemoveFromWorld(tWorld *world)
{
	world->RemoveCubeMapReflection(reflection);
}
