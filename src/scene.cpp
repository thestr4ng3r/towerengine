
#include "towerengine.h"


#include "rapidxml.hpp"


using namespace std;
using namespace rapidxml;


tScene::tScene(tWorld *world, tMaterialManager *material_manager)
{
	sky_cubemap = 0;
	skybox = 0;
	this->world = world;

	if(material_manager)
	{
		this->material_manager = material_manager;
		own_material_manager = false;
	}
	else
	{
		this->material_manager = new tMaterialManager();
		own_material_manager = true;
	}
}

tScene::~tScene(void)
{
	// TODO: delete assets, objects, ...
	if(own_material_manager)
		delete material_manager;
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

	load_path = PathOfFile(file);

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
		else if(strcmp(child->name(), "material") == 0)
			ParseMaterialAssetNode(child);
		else if(strcmp(child->name(), "cubemap") == 0)
			ParseCubeMapAssetNode(child);

		child = child->next_sibling();
	}
}

void tScene::ParseMeshAssetNode(xml_node<> *cur)
{
	char *file_data = 0;
	string file;
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
			if((attr = child->first_attribute("file")))
			{
				file = string(attr->value());
				break; // remove this break if other things should be loaded
			}
			else if(child->value_size() > 0)
			{
				file_data = child->value();
				break; // remove this break if other things should be loaded
			}
		}
		child = child->next_sibling();
	}


	tMesh *mesh;
	if(file_data)
	{
		mesh = new tMesh();
		mesh->LoadFromData(file_data, "", material_manager);
	}
	else if(file.size() > 0)
	{
		mesh = new tMesh();
		mesh->LoadFromFile((load_path + "/" + file).c_str(), material_manager);
	}
	else
		return;

	assets.insert(pair<string, tAsset *>(name, (tAsset *)(new tMeshAsset(mesh))));
}

void tScene::ParseMaterialAssetNode(xml_node<> *cur)
{
	string name;
	tMaterial *mat;

	mat = tMesh::ParseXMLMaterialNode(cur, name, load_path);

	if(mat)
	{
		try
		{
			material_manager->AddMaterial(name, mat);
		}
		catch(std::exception &ex)
		{
			printf("exception: %s\n", ex.what());
		}
	}
}

void tScene::ParseCubeMapAssetNode(xml_node<> *cur)
{
	xml_attribute<> *attr;
	char *base64_temp;
	size_t file_size;
	unsigned char *file_data = 0;
	const char *file_ext = 0;
	string name;
	string file;

	if(!(attr = cur->first_attribute("name")))
			return;
	name = string(attr->value());

	xml_node<> *child = cur->first_node();
	while(child)
	{
		if(strcmp(child->name(), "data") == 0)
		{
			if((attr = child->first_attribute("file")))
			{
				file = string(attr->value());
				break; // remove this break if other things should be loaded
			}
			else if(child->value_size() > 0)
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

	GLuint cubemap;

	if(file_data)
		cubemap = LoadGLCubeMapBinary(file_ext, file_data, file_size);
	else
	{
		cubemap = LoadGLCubeMap((load_path + "/" + file).c_str());
	}
	assets.insert(pair<string, tAsset *>(name, (tAsset *)(new tCubeMapAsset(cubemap))));
}

void tScene::ParseObjectsNode(xml_node<> *cur)
{
	for(xml_node<> *child = cur->first_node(); child; child=child->next_sibling())
	{
		tSceneObject *scene_object = 0;
		xml_attribute<> *attr;

		if(!(attr = child->first_attribute("name")))
			continue;
		string name = string(attr->value());

		if(strcmp(child->name(), "mesh") == 0)
			scene_object = ParseMeshObjectNode(child);
		else if(strcmp(child->name(), "dir_light") == 0)
			scene_object = ParseDirectionalLightObjectNode(child);
		else if(strcmp(child->name(), "point_light") == 0)
			scene_object = ParsePointLightObjectNode(child);
		else if(strcmp(child->name(), "empty") == 0)
			scene_object = ParseEmptyObjectNode(child);

		if(!scene_object)
			continue;

		if((attr = child->first_attribute("tag")))
			scene_object->SetTag(string(attr->value()));

		for(xml_node<> *child3 = child->first_node("attribute"); child3; child3=child3->next_sibling("attribute"))
		{
			if(!(attr = child3->first_attribute("name")))
				continue;
			string attr_name = string(attr->value());

			if(!(attr = child3->first_attribute("value")))
				continue;
			string attr_value = string(attr->value());

			scene_object->SetAttribute(attr_name, attr_value);
		}

		AddObject(name, scene_object);
	}
}

tSceneObject *tScene::ParseMeshObjectNode(xml_node<> *cur)
{
	string mesh_asset_name;
	tTransform transform;
	xml_attribute<> *attr;
	bool rigid_body_enabled = false;
	float rigid_body_mass = 0.0;

	xml_node<> *child = cur->first_node();
	for(; child; child = child->next_sibling())
	{
		if(strcmp(child->name(), "mesh_asset") == 0)
		{
			if((attr = child->first_attribute("asset")))
				mesh_asset_name = string(attr->value());
		}
		else if(strcmp(child->name(), "transform") == 0)
			transform = ParseTransformNode(child);
		else if(strcmp(child->name(), "rigid_body") == 0)
		{
			if((attr = child->first_attribute("mass")))
				rigid_body_mass = atof(attr->value());
			else
				continue;

			rigid_body_enabled = true;
		}
	}

	map<string, tAsset *>::iterator asset_i;
	tAsset *asset;

	if((asset_i = assets.find(mesh_asset_name)) == assets.end())
		return 0;
	asset = asset_i->second;

	if(asset->GetType() != T_ASSET_TYPE_MESH)
		return 0;

	tMeshObject *object = new tMeshObject(((tMeshAsset *)asset)->GetMesh());
	//object->SetTag(tag);
	object->SetTransform(transform);
	if(rigid_body_enabled)
	{
		object->InitMeshRigidBody(rigid_body_mass);
		object->UpdateRigidBodyTransformation();
	}
	tObjectSceneObject *scene_object = new tObjectSceneObject(object);




	return scene_object;
}

tSceneObject *tScene::ParseDirectionalLightObjectNode(xml_node<> *cur)
{
	tVector direction, color;

	for(xml_node<> *child=cur->first_node(); child; child=child->next_sibling())
	{
		if(strcmp(child->name(), "direction") == 0)
			direction = ParseVectorNode(child);
		else if(strcmp(child->name(), "color") == 0)
			color = ParseVectorNode(child, "r", "g", "b");
	}

	tDirectionalLight *dir_light = new tDirectionalLight(direction, color);
	tDirectionalLightSceneObject *scene_object = new tDirectionalLightSceneObject(dir_light);

	return scene_object;
}

tSceneObject *tScene::ParsePointLightObjectNode(xml_node<> *cur)
{
	tVector position, color;
	float distance = 5.0;

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

	return scene_object;
}

tSceneObject *tScene::ParseEmptyObjectNode(xml_node<> *cur)
{
	tTransform transform;
	xml_attribute<> *attr;

	bool cube_map_reflection = false;

	if((attr = cur->first_attribute("tag")))
		if(strcmp(attr->value(), "T_cube_map_reflection") == 0)
			cube_map_reflection = true;

	xml_node<> *child = cur->first_node();
	for(; child; child = child->next_sibling())
	{
		if(strcmp(child->name(), "transform") == 0)
			transform = ParseTransformNode(child);
	}

	if(!cube_map_reflection)
	{
		tEmptySceneObject *scene_object = new tEmptySceneObject(transform);
		return scene_object;
	}
	else
	{
		tCubeMapReflection *reflection = new tCubeMapReflection(transform.GetPosition());
		tCubeMapReflectionSceneObject *scene_object = new tCubeMapReflectionSceneObject(reflection);
		return scene_object;
	}
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

	world->AssignUnsetCubeMapReflections();
}

void tScene::RemoveFromWorld(void)
{
	map<string, tSceneObject *>::iterator i;

	world->SetSkyBox(0);

	for(i=objects.begin(); i!=objects.end(); i++)
		i->second->RemoveFromWorld(world);
}


tSceneObject *tScene::GetObjectByTag(string tag)
{
	map<string, tSceneObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(i->second->GetTag().compare(tag) == 0)
			return i->second;

	return 0;
}

void tScene::GetObjectsByTag(string tag, list<tSceneObject *> &result)
{
	map<string, tSceneObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(i->second->GetTag().compare(tag) == 0)
			result.push_back(i->second);
}


tSceneObject *tScene::GetObjectWhereTagStartsWith(string start)
{
	map<string, tSceneObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(i->second->GetTag().substr(0, start.size()).compare(start) == 0)
			return i->second;

	return 0;
}


void tScene::GetObjectsWhereTagStartsWith(string start, list<tSceneObject *> &result)
{
	map<string, tSceneObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(i->second->GetTag().substr(0, start.size()).compare(start) == 0)
			result.push_back(i->second);
}

void tScene::GetObjectsMapByTag(string tag, string attribute, multimap<string, tSceneObject*> &result)
{
	map<string, tSceneObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(i->second->GetTag().compare(tag) == 0)
		{
			tSceneObject *object = i->second;
			string attr_value = object->GetAttribute(attribute);
			result.insert(pair<string, tSceneObject *>(attr_value, object));
		}
}





