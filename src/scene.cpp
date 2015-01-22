
#include "towerengine.h"


#include "rapidxml.hpp"

using namespace rapidxml;


tScene::tScene(tWorld *world)
{
	sky_cubemap = 0;
	skybox = 0;
	this->world = world;
}

tScene::~tScene(void)
{

}

bool tScene::LoadFromFile(string file)
{
	char * data = ReadFile(file.c_str());
	if(!data)
		return false;

	xml_document<> *doc = new xml_document<>();
	doc->parse<0>(data);

	if(!doc)
		return false;

	xml_node<> *cur;

	cur = doc->first_node();
	if(!cur)
		return false;

	if(strcmp(cur->name(), "tscene") != 0)
		return false;


	cur = cur->first_node();

	while(cur)
	{
		if(strcmp(cur->name(), "assets") == 0)
			ParseAssetsNode(cur);
		else if(strcmp(cur->name(), "objects") == 0)
			ParseObjectsNode(cur);
		else if(strcmp(cur->name(), "scene") == 0)
			ParseSceneNode(cur);

		cur = cur->next_sibling();
	}

	delete doc;
	delete [] data;

	return true;
}

void tScene::ParseAssetsNode(xml_node<> *cur)
{
	xml_node<> *child = cur->first_node();
	string name;


	while(child)
	{
		if(strcmp(child->name(), "mesh") == 0)
			ParseMeshAssetNode(child);
		else if(strcmp(child->name(), "cubemap") == 0)
			ParseCubeMapAssetNode(child);

		child = child->next_sibling();
	}
}

void tScene::ParseMeshAssetNode(xml_node<> *cur)
{
	char *file_data = 0;
	xml_attribute<> *attr;
	string name;

	if(!(attr = cur->first_attribute("name")))
		return;
	name = string(attr->value());

	xml_node<> *child = cur->first_node();
	while(child)
	{
		if(strcmp(child->name(), "data") == 0)
		{
			if(child->value_size() > 0)
			{
				file_data = child->value();
				break; // remove this break if other things should be loaded
			}
		}
		child = child->next_sibling();
	}

	if(!file_data)
		return;


	tMesh *mesh = new tMesh();
	mesh->LoadFromData(file_data, "");

	assets.insert(pair<string, tAsset *>(name, (tAsset *)(new tMeshAsset(mesh))));
}

void tScene::ParseCubeMapAssetNode(xml_node<> *cur)
{
	xml_attribute<> *attr;
	char *base64_temp;
	unsigned long int file_size;
	unsigned char *file_data = 0;
	const char *file_ext = 0;
	string name;

	if(!(attr = cur->first_attribute("name")))
			return;
	name = string(attr->value());

	xml_node<> *child = cur->first_node();
	while(child)
	{
		if(strcmp(child->name(), "data") == 0)
		{
			if(child->value_size() > 0)
			{
				base64_temp = child->value();
				Base64Decode(base64_temp, &file_data, &file_size);

				attr = child->first_attribute("filename_ext");
				if(attr)
					file_ext = attr->value();
				break; // remove this break if other things should be loaded
			}
		}
		child = child->next_sibling();
	}

	if(!file_data)
		return;

	GLuint cubemap = LoadGLCubeMapBinary(file_ext, file_data, file_size);
	assets.insert(pair<string, tAsset *>(name, (tAsset *)(new tCubeMapAsset(cubemap))));
}

void tScene::ParseObjectsNode(xml_node<> *cur)
{
	for(xml_node<> *child = cur->first_node(); child; child=child->next_sibling())
	{
		if(strcmp(child->name(), "mesh") == 0)
			ParseMeshObjectNode(child);
		else if(strcmp(child->name(), "dir_light") == 0)
			ParseDirectionalLightObjectNode(child);
		else if(strcmp(child->name(), "point_light") == 0)
			ParsePointLightObjectNode(child);
	}
}

void tScene::ParseMeshObjectNode(xml_node<> *cur)
{
	string name;
	string mesh_asset_name;
	tTransform transform;
	xml_attribute<> *attr;


	if(!(attr = cur->first_attribute("name")))
		return;
	name = string(attr->value());


	xml_node<> *child = cur->first_node();
	while(child)
	{
		if(strcmp(child->name(), "mesh_asset") == 0)
		{
			if((attr = child->first_attribute("asset")))
				mesh_asset_name = string(attr->value());
		}
		else if(strcmp(child->name(), "transform") == 0)
			transform = ParseTransformNode(child);

		child = child->next_sibling();
	}

	map<string, tAsset *>::iterator asset_i;
	tAsset *asset;

	if((asset_i = assets.find(mesh_asset_name)) == assets.end())
		return;
	asset = asset_i->second;

	if(asset->GetType() != T_ASSET_TYPE_MESH)
		return;

	tMeshObject *object = new tMeshObject(((tMeshAsset *)asset)->GetMesh());
	object->SetTransform(transform);
	object->UpdateRigidBodyTransformation();
	tObjectSceneObject *scene_object = new tObjectSceneObject(object);
	AddObject(name, scene_object);
}

void tScene::ParseDirectionalLightObjectNode(xml_node<> *cur)
{
	string name;
	tVector direction, color;
	xml_attribute<> *attr;

	if(!(attr = cur->first_attribute("name")))
		return;
	name = string(attr->value());


	for(xml_node<> *child=cur->first_node(); child; child=child->next_sibling())
	{
		if(strcmp(child->name(), "direction") == 0)
			direction = ParseVectorNode(child);
		else if(strcmp(child->name(), "color") == 0)
			color = ParseVectorNode(child, "r", "g", "b");
	}

	tDirectionalLight *dir_light = new tDirectionalLight(direction, color);
	tDirectionalLightSceneObject *scene_object = new tDirectionalLightSceneObject(dir_light);
	AddObject(name, scene_object);
}

void tScene::ParsePointLightObjectNode(xml_node<> *cur)
{
	string name;
	tVector position, color;
	float distance = 5.0;
	xml_attribute<> *attr;

	if(!(attr = cur->first_attribute("name")))
		return;
	name = string(attr->value());

	for(xml_node<> *child=cur->first_node(); child; child=child->next_sibling())
	{
		if(strcmp(child->name(), "position") == 0)
			position = ParseVectorNode(child);
		else if(strcmp(child->name(), "color") == 0)
			color = ParseVectorNode(child, "r", "g", "b");
		else if(strcmp(child->name(), "distance") == 0)
			distance = ParseFloatNode(child);
	}

	tPointLight *point_light = new tPointLight(position, color, distance);
	tPointLightSceneObject *scene_object = new tPointLightSceneObject(point_light);
	AddObject(name, scene_object);
}

void tScene::ParseSceneNode(xml_node<> *cur)
{
	map<string, tAsset *>::iterator asset_i;
	string asset_name;
	xml_attribute<> *attr;


	if((attr = cur->first_attribute("sky_cubemap")))
	{
		asset_name = string(attr->value());

		if((asset_i = assets.find(asset_name)) != assets.end())
		{
			tAsset *asset = asset_i->second;

			if(asset->GetType() == T_ASSET_TYPE_CUBE_MAP)
			{
				sky_cubemap = (tCubeMapAsset *)asset;
				skybox = new tSkyBox(sky_cubemap->GetCubeMap());
			}
		}
	}
}



float tScene::ParseFloatNode(xml_node<> *node, const char *p)
{
	float r = 0.0;

	xml_attribute<> *attr = node->first_attribute(p);
	if(attr)
		r = atof(attr->value());

	return r;
}


tTransform tScene::ParseTransformNode(xml_node<> *cur)
{
	tVector position;
	tMatrix3 basis;

	for(xml_node<> *child=cur->first_node(); child; child=child->next_sibling())
	{
		if(strcmp(child->name(), "position") == 0)
			position = ParseVectorNode(child);
		else if(strcmp(child->name(), "basis_x") == 0)
			basis.SetX(ParseVectorNode(child));
		else if(strcmp(child->name(), "basis_y") == 0)
			basis.SetY(ParseVectorNode(child));
		else if(strcmp(child->name(), "basis_z") == 0)
			basis.SetZ(ParseVectorNode(child));
	}

	return tTransform(basis, position);
}

tVector tScene::ParseVectorNode(xml_node<> *cur, const char *x_p, const char *y_p, const char *z_p)
{
	tVector r = Vec(0.0, 0.0, 0.0);

	xml_attribute<> *attr;

	if((attr = cur->first_attribute(x_p)))
		r.x = atof(attr->value());

	if((attr = cur->first_attribute(y_p)))
		r.y = atof(attr->value());

	if((attr = cur->first_attribute(z_p)))
		r.z = atof(attr->value());

	return r;
}

void tScene::AddObject(string name, tSceneObject *object)
{
	objects.insert(pair<string, tSceneObject *>(name, object));
}

void tScene::AddToWorld(void)
{
	map<string, tSceneObject *>::iterator i;

	world->SetSkyBox(skybox);

	for(i=objects.begin(); i!=objects.end(); i++)
		i->second->AddToWorld(world);

}

void tScene::RemoveFromWorld(void)
{
	map<string, tSceneObject *>::iterator i;

	world->SetSkyBox(0);

	for(i=objects.begin(); i!=objects.end(); i++)
		i->second->RemoveFromWorld(world);
}

