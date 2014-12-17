
#include "towerengine.h"

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
		else if(xmlStrEqual(cur->name, (const xmlChar *)"scene"))
			ParseSceneNode(cur);

		cur = cur->next;
	}

	xmlFreeDoc(doc);

	return true;
}

void tScene::ParseAssetsNode(xmlNodePtr cur)
{
	xmlNodePtr child = cur->children;
	string name;

	while(child)
	{
		if(xmlStrEqual(child->name, (const xmlChar *)"mesh"))
			ParseMeshAssetNode(child);
		else if(xmlStrEqual(child->name, (const xmlChar *)"cubemap"))
			ParseCubeMapAssetNode(child);

		child = child->next;
	}
}

void tScene::ParseMeshAssetNode(xmlNodePtr cur)
{
	xmlChar *temp;
	unsigned char *file_data = 0;
	xmlNodePtr child = cur->children;
	string name;

	if(!(temp = xmlGetProp(cur, (const xmlChar *)"name")))
			return;
	name = string((const char *)temp);

	while(child)
	{
		if(xmlStrEqual(child->name, (const xmlChar *)"data"))
		{
			if((file_data = (unsigned char *)xmlNodeListGetString(child->doc, child->children, 1)))
			{
				break; // remove this break if other things should be loaded
			}
		}
		child = child->next;
	}

	if(!file_data)
		return;

	tMesh *mesh = new tMesh();
	mesh->LoadFromData((const char *)file_data, "");

	assets.insert(pair<string, tAsset *>(name, (tAsset *)(new tMeshAsset(mesh))));
}

void tScene::ParseCubeMapAssetNode(xmlNodePtr cur)
{
	xmlChar *temp;
	char *base64_temp;
	unsigned long int file_size;
	unsigned char *file_data = 0;
	const char *file_ext = "";
	string name;

	if(!(temp = xmlGetProp(cur, (const xmlChar *)"name")))
			return;
	name = string((const char *)temp);

	xmlNodePtr child = cur->children;

	while(child)
	{
		if(xmlStrEqual(child->name, (const xmlChar *)"data"))
		{
			if((base64_temp = (char *)xmlNodeListGetString(child->doc, child->children, 1)))
			{
				Base64Decode(base64_temp, &file_data, &file_size);
				file_ext = (const char *)xmlGetProp(child, (const xmlChar *)"filename_ext");
				break; // remove this break if other things should be loaded
			}
		}
		child = child->next;
	}

	if(!file_data)
		return;

	GLuint cubemap = LoadGLCubeMapBinary(file_ext, file_data, file_size);
	assets.insert(pair<string, tAsset *>(name, (tAsset *)(new tCubeMapAsset(cubemap))));
}

void tScene::ParseObjectsNode(xmlNodePtr cur)
{
	for(xmlNodePtr child = cur->children; child; child=child->next)
	{
		if(xmlStrEqual(child->name, (const xmlChar *)"mesh"))
			ParseMeshObjectNode(child);
		else if(xmlStrEqual(child->name, (const xmlChar *)"dir_light"))
			ParseDirectionalLightObjectNode(child);
		else if(xmlStrEqual(child->name, (const xmlChar *)"point_light"))
			ParsePointLightObjectNode(child);
	}
}

void tScene::ParseMeshObjectNode(xmlNodePtr cur)
{
	xmlChar *temp;
	string name;
	string mesh_asset_name;
	tTransform transform;

	if(!(temp = xmlGetProp(cur, (const xmlChar *)"name")))
		return;
	name = string((const char *)temp);


	xmlNodePtr child = cur->children;
	while(child)
	{
		if(xmlStrEqual(child->name, (const xmlChar *)"mesh_asset"))
		{
			if((temp = xmlGetProp(child, (const xmlChar *)"asset")))
			{
				mesh_asset_name = string((const char *)temp);
			}
		}
		else if(xmlStrEqual(child->name, (const xmlChar *)"transform"))
			transform = ParseTransformNode(child);

		child = child->next;
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

void tScene::ParseDirectionalLightObjectNode(xmlNodePtr cur)
{
	xmlChar *temp;
	string name;
	tVector direction, color;

	if(!(temp = xmlGetProp(cur, (const xmlChar *)"name")))
		return;
	name = string((const char *)temp);


	for(xmlNodePtr child=cur->children; child; child=child->next)
	{
		if(xmlStrEqual(child->name, (const xmlChar *)"direction"))
			direction = ParseVectorNode(child);
		else if(xmlStrEqual(child->name, (const xmlChar *)"color"))
			color = ParseVectorNode(child, (const xmlChar *)"r", (const xmlChar *)"g", (const xmlChar *)"b");
	}

	tDirectionalLight *dir_light = new tDirectionalLight(direction, color);
	tDirectionalLightSceneObject *scene_object = new tDirectionalLightSceneObject(dir_light);
	AddObject(name, scene_object);
}

void tScene::ParsePointLightObjectNode(xmlNodePtr cur)
{
	xmlChar *temp;
	string name;
	tVector position, color;
	float distance = 5.0;

	if(!(temp = xmlGetProp(cur, (const xmlChar *)"name")))
		return;
	name = string((const char *)temp);

	for(xmlNodePtr child=cur->children; child; child=child->next)
	{
		if(xmlStrEqual(child->name, (const xmlChar *)"position"))
			position = ParseVectorNode(child);
		else if(xmlStrEqual(child->name, (const xmlChar *)"color"))
			color = ParseVectorNode(child, (const xmlChar *)"r", (const xmlChar *)"g", (const xmlChar *)"b");
		else if(xmlStrEqual(child->name, (const xmlChar *)"distance"))
			distance = ParseFloatNode(child);
	}

	tPointLight *point_light = new tPointLight(position, color, distance);
	tPointLightSceneObject *scene_object = new tPointLightSceneObject(point_light);
	AddObject(name, scene_object);
}

void tScene::ParseSceneNode(xmlNodePtr cur)
{
	xmlChar *temp;
	map<string, tAsset *>::iterator asset_i;
	string asset_name;

	if((temp = xmlGetProp(cur, (const xmlChar *)"sky_cubemap")))
	{
		asset_name = string((const char *)temp);

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



float tScene::ParseFloatNode(xmlNodePtr node, const xmlChar *p)
{
	float r = 0.0;

	xmlChar *temp;

	if((temp = xmlGetProp(node, p)))
		r = atof((const char *)temp);

	return r;
}


tTransform tScene::ParseTransformNode(xmlNodePtr cur)
{
	tVector position;
	tMatrix3 basis;


	for(xmlNodePtr child=cur->children; child; child=child->next)
	{
		if(xmlStrEqual(child->name, (const xmlChar *)"position"))
			position = ParseVectorNode(child);
		else if(xmlStrEqual(child->name, (const xmlChar *)"basis_x"))
			basis.SetX(ParseVectorNode(child));
		else if(xmlStrEqual(child->name, (const xmlChar *)"basis_y"))
			basis.SetY(ParseVectorNode(child));
		else if(xmlStrEqual(child->name, (const xmlChar *)"basis_z"))
			basis.SetZ(ParseVectorNode(child));
	}

	return tTransform(basis, position);
}

tVector tScene::ParseVectorNode(xmlNodePtr cur, const xmlChar *x_p, const xmlChar *y_p, const xmlChar *z_p)
{
	tVector r = Vec(0.0, 0.0, 0.0);

	xmlChar *temp;

	if((temp = xmlGetProp(cur, x_p)))
		r.x = atof((const char *)temp);

	if((temp = xmlGetProp(cur, y_p)))
		r.y = atof((const char *)temp);

	if((temp = xmlGetProp(cur, z_p)))
		r.z = atof((const char *)temp);

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

