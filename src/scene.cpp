
#include "towerengine.h"

tScene::tScene(tWorld *world)
{
	this->world = world;
}

tScene::~tScene(void)
{

}

bool tScene::LoadFromFile(string file)
{
	xmlDocPtr doc = xmlReadFile(file.data(), 0, XML_PARSE_HUGE);

	if(!doc)
		return false;

	xmlNodePtr cur;

	cur = xmlDocGetRootElement(doc);
	if(!cur)
		return false;

	if(!xmlStrEqual(cur->name, (const xmlChar *)"tscene"))
		return false;



	cur = cur->children;

	while(cur)
	{
		if(xmlStrEqual(cur->name, (const xmlChar *)"assets"))
			ParseAssetsNode(cur);

		else if(xmlStrEqual(cur->name, (const xmlChar *)"objects"))
			ParseObjectsNode(cur);

		cur = cur->next;
	}

	return true;
}

void tScene::ParseAssetsNode(xmlNodePtr cur)
{
	xmlNodePtr child = cur->children;
	char *base64_temp;
	unsigned char *file_data;
	unsigned long int file_size;
	string name;

	while(child)
	{
		if(xmlStrEqual(child->name, (const xmlChar *)"mesh"))
		{
			if((base64_temp = (char *)xmlNodeListGetString(child->doc, child->children, 1)))
			{
				Base64Decode(base64_temp, &file_data, &file_size);
				tMesh *mesh = new tMesh();
				mesh->LoadFromData((const char *)file_data, "");
				name = string((const char *)xmlGetProp(child, (const xmlChar *)"name"));

				assets.insert(pair<string, tAsset *>(name, (tAsset *)(new tMeshAsset(mesh))));
			}
		}

		child = child->next;
	}
}

void tScene::ParseObjectsNode(xmlNodePtr cur)
{
	xmlChar *temp;
	string name;
	string mesh_name;
	tVector pos, dir;
	float distance;
	tVector color;
	map<string, tAsset *>::iterator asset_i;
	tAsset *asset;

	for(xmlNodePtr child = cur->children; child; child=child->next)
	{
		if(xmlStrEqual(child->name, (const xmlChar *)"mesh"))
		{
			if(!(temp = xmlGetProp(child, (const xmlChar *)"name")))
				continue;
			name = string((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"mesh")))
				continue;
			mesh_name = string((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"x")))
				continue;
			pos.x = atof((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"y")))
					continue;
			pos.y = atof((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"z")))
				continue;
			pos.z = atof((const char *)temp);

			if((asset_i = assets.find(mesh_name)) == assets.end())
				continue;
			asset = asset_i->second;

			if(asset->GetType() != T_ASSET_TYPE_MESH)
				continue;

			tMeshObject *object = new tMeshObject(((tMeshAsset *)asset)->GetMesh());
			object->SetPosition(pos);
			tObjectSceneObject *scene_object = new tObjectSceneObject(object);
			AddObject(name, scene_object);
		}

		if(xmlStrEqual(child->name, (const xmlChar *)"dir_light"))
		{
			if(!(temp = xmlGetProp(child, (const xmlChar *)"name")))
				continue;
			name = string((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"x")))
				continue;
			dir.x = atof((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"y")))
					continue;
			dir.y = atof((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"z")))
				continue;
			dir.z = atof((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"r")))
				continue;
			color.x = atof((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"g")))
					continue;
			color.y = atof((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"b")))
				continue;
			color.z = atof((const char *)temp);

			tDirectionalLight *dir_light = new tDirectionalLight(dir, color);
			tDirectionalLightSceneObject *scene_object = new tDirectionalLightSceneObject(dir_light);
			AddObject(name, scene_object);
		}

		if(xmlStrEqual(child->name, (const xmlChar *)"point_light"))
		{
			if(!(temp = xmlGetProp(child, (const xmlChar *)"name")))
				continue;
			name = string((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"x")))
				continue;
			pos.x = atof((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"y")))
					continue;
			pos.y = atof((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"z")))
				continue;
			pos.z = atof((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"r")))
				continue;
			color.x = atof((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"g")))
					continue;
			color.y = atof((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"b")))
				continue;
			color.z = atof((const char *)temp);

			if(!(temp = xmlGetProp(child, (const xmlChar *)"dist")))
				continue;
			distance = atof((const char *)temp);

			tPointLight *point_light = new tPointLight(pos, color, distance);
			tPointLightSceneObject *scene_object = new tPointLightSceneObject(point_light);
			AddObject(name, scene_object);
		}
	}
}

void tScene::AddObject(string name, tSceneObject *object)
{
	objects.insert(pair<string, tSceneObject *>(name, object));
}

void tScene::AddToWorld(void)
{
	map<string, tSceneObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		i->second->AddToWorld(world);
}

void tScene::RemoveFromWorld(void)
{
	map<string, tSceneObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		i->second->RemoveFromWorld(world);
}

