
#include "towerengine.h"


#include "rapidxml.hpp"

#include <string>
#include <sstream>

using namespace std;
using namespace rapidxml;



template<typename T> T inline GetXMLAttribute(rapidxml::xml_node<char> *node, const char *attr_name, T default_v)
{
	rapidxml::xml_attribute<> *attr = node->first_attribute(attr_name);

	if(!attr)
		return default_v;

	std::string str = attr->value();
	std::istringstream iss(str);
	T v;

	iss >> v;

	if(!iss.eof())
		return default_v;

	return v;
}




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
	//printf("delete scene\n");
	for(map<string, tSceneObject *>::iterator i=objects.begin(); i!=objects.end(); i++)
		delete i->second;

	//printf("deleted objects\n");

	for(map<string, tAsset *>::iterator i=assets.begin(); i!=assets.end(); i++)
	{
		//printf("asset %d\n", ai++);
		delete i->second;
	}

	//printf("deleted assets\n");

	if(own_material_manager)
		delete material_manager;
	//printf("deleted material manager\n");

	delete skybox;

	//printf("deleted skybox\n");
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


	for(map<string, tSceneObject *>::iterator i=objects.begin(); i!=objects.end(); i++)
	{
		tSceneObject *so = i->second;
		string reflection_name = so->GetAttribute("T_reflection_probe");

		if(reflection_name.empty())
			continue;

		tObjectSceneObject *oso = dynamic_cast<tObjectSceneObject *>(so);
		if(!oso)
			continue;

		tMeshObject *mesh_object = dynamic_cast<tMeshObject *>(oso->GetObject());
		if(!mesh_object)
			continue;

		map<string, tReflectionProbeSceneObject *>::iterator reflection_i = reflection_probe_scene_objects.find(reflection_name);
		if(reflection_i == reflection_probe_scene_objects.end())
			continue;

		mesh_object->SetCubeMapReflection(reflection_i->second->GetReflection());
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
		cubemap = LoadGLCubeMapBinary(file_ext, file_data, (unsigned int)file_size);
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

			if(scene_object->GetType() == T_SCENE_OBJECT_TYPE_REFLECTION_PROBE && attr_name == "T_cube_map_reflection_name")
			{
				tReflectionProbeSceneObject *cube_map_reflection_object = dynamic_cast<tReflectionProbeSceneObject *>(scene_object);
				if(!cube_map_reflection_object)
					continue;

				reflection_probe_scene_objects.insert(pair<string, tReflectionProbeSceneObject *>(attr_value, cube_map_reflection_object));
			}
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
	enum { MESH, CONVEX, BOX } collision_shape_type = MESH;
	tVector collision_shape_half_extents;

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
				rigid_body_mass = (float)atof(attr->value());
			else
				continue;

			xml_node<> *collision_shape_node = child->first_node("collision_shape");

			if(collision_shape_node)
			{
				if((attr = collision_shape_node->first_attribute("type")))
				{
					if(strcmp(attr->value(), "mesh") == 0)
						collision_shape_type = MESH;
					else if(strcmp(attr->value(), "convex") == 0)
						collision_shape_type = CONVEX;
					else if(strcmp(attr->value(), "box") == 0)
						collision_shape_type = BOX;
				}

				if(collision_shape_type == BOX)
				{
					xml_node<> *half_extents_node = collision_shape_node->first_node("half_extents");
					if(half_extents_node)
					{
						collision_shape_half_extents.x = GetXMLAttribute<float>(half_extents_node, "x", 0.0);
						collision_shape_half_extents.y = GetXMLAttribute<float>(half_extents_node, "y", 0.0);
						collision_shape_half_extents.z = GetXMLAttribute<float>(half_extents_node, "z", 0.0);
					}
				}
			}

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
		if(collision_shape_type == MESH || collision_shape_type == CONVEX)
			object->InitMeshRigidBody(rigid_body_mass, collision_shape_type == CONVEX);
		else if(collision_shape_type == BOX)
			object->InitBoxRigidBody(collision_shape_half_extents, rigid_body_mass);
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
	tVector extent_a = tVec(1.0, 1.0, 1.0);
	tVector extent_b = tVec(-1.0, -1.0, -1.0);
	tVector delta_position = tVec(0.0, 0.0, 0.0);
	xml_attribute<> *attr;

	bool reflection_probe = false;

	if((attr = cur->first_attribute("tag")))
		if(strcmp(attr->value(), "T_reflection_probe") == 0)
			reflection_probe = true;

	xml_node<> *child = cur->first_node();
	for(; child; child = child->next_sibling())
	{
		if(strcmp(child->name(), "transform") == 0)
			transform = ParseTransformNode(child);
		else if(strcmp(child->name(), "delta_transform") == 0)
		{
			tVector scale = tVec(1.0, 1.0, 1.0);

			for(xml_node<> *child2=child->first_node(); child2; child2=child2->next_sibling())
			{
				if(strcmp(child2->name(), "position") == 0)
					delta_position = ParseVectorNode(child2);
				else if(strcmp(child2->name(), "scale") == 0)
					scale = ParseVectorNode(child2);
			}

			extent_a = scale + delta_position;
			extent_b = -scale + delta_position;
		}
	}

	transform.SetPosition(transform.GetPosition() - delta_position);

	if(!reflection_probe)
	{
		tEmptySceneObject *scene_object = new tEmptySceneObject(transform);
		return scene_object;
	}
	else
	{
		tReflectionProbe *reflection = new tReflectionProbe(transform.GetPosition(), extent_a, extent_b);
		tReflectionProbeSceneObject *scene_object = new tReflectionProbeSceneObject(reflection);
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
				delete skybox;
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
		r = (float)atof(attr->value());

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
			basis.x = ParseVectorNode(child);
		else if(strcmp(child->name(), "basis_y") == 0)
			basis.y = ParseVectorNode(child);
		else if(strcmp(child->name(), "basis_z") == 0)
			basis.z = ParseVectorNode(child);
	}

	return tTransform(basis, position);
}

tVector tScene::ParseVectorNode(xml_node<> *cur, const char *x_p, const char *y_p, const char *z_p)
{
	tVector r = tVec(0.0, 0.0, 0.0);

	xml_attribute<> *attr;

	if((attr = cur->first_attribute(x_p)))
		r.x = (float)atof(attr->value());

	if((attr = cur->first_attribute(y_p)))
		r.y = (float)atof(attr->value());

	if((attr = cur->first_attribute(z_p)))
		r.z = (float)atof(attr->value());

	return r;
}

void tScene::AddObject(string name, tSceneObject *object)
{
	//printf("AddObject %s\n", name.c_str());
	objects.insert(pair<string, tSceneObject *>(name, object));
}

void tScene::AddToWorld(void)
{
	map<string, tSceneObject *>::iterator i;

	world->SetSkyBox(skybox);

	for(i=objects.begin(); i!=objects.end(); i++)
		i->second->AddToWorld(world);

	world->AssignUnsetReflectionProbes();
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





