
#include "towerengine.h"

CScene::CScene(CWorld *world)
{
	this->world = world;
}

CScene::~CScene(void)
{

}

bool CScene::LoadFromFile(string file)
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

void CScene::ParseAssetsNode(xmlNodePtr cur)
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
				CMesh *mesh = new CMesh();
				mesh->LoadFromData((const char *)file_data, "");
				name = string((const char *)xmlGetProp(child, (const xmlChar *)"name"));
				meshes.insert(pair<string, CMesh *>(name, mesh));
			}
		}

		child = child->next;
	}
}

void CScene::ParseObjectsNode(xmlNodePtr cur)
{
	xmlChar *temp;
	string name;
	string mesh_name;
	CVector pos;
	//CVector color;
	map<string, CMesh *>::iterator mesh_i;

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

			if((mesh_i = meshes.find(mesh_name)) == meshes.end())
				continue;

			CMeshObject *object = new CMeshObject(mesh_i->second);
			object->SetPosition(pos);
			CObjectSceneObject *scene_object = new CObjectSceneObject(object);
			objects.insert(pair<string, CSceneObject *>(name, (CSceneObject *)scene_object));
		}

		if(xmlStrEqual(child->name, (const xmlChar *)"dir_light"))
		{
		}

		if(xmlStrEqual(child->name, (const xmlChar *)"point_light"))
		{
		}
	}
}

void CScene::AddToWorld(void)
{
	map<string, CSceneObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
	{
		printf("%ld\n", (long)i->second);
		i->second->AddToWorld(world);
		printf("adding object %s\n", i->first.data());
	}
}

void CScene::RemoveFromWorld(void)
{
	map<string, CSceneObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		i->second->RemoveFromWorld(world);
}

