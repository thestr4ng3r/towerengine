#include "towerengine.h"

#include "rapidxml.hpp"


using namespace std;
using namespace rapidxml;


tMesh::tMesh(const char *file, tMaterialManager *material_manager)
{
	vao = 0;
	physics_triangle_mesh = 0;
	bounding_box = tBoundingBox();

	refresh_vbos = true;
	refresh_ibos = true;

	vao = new tVAO();
	vertex_vbo = new tVBO<float>(3);
	normal_vbo = new tVBO<float>(3);
	tang_vbo = new tVBO<float>(3);
	bitang_vbo = new tVBO<float>(3);
	face_normal_vbo = new tVBO<float>(3);
	uvcoord_vbo = new tVBO<float>(2);

	vao->Bind();
	vertex_vbo->SetAttribute(tFaceShader::vertex_attribute, GL_FLOAT);
	normal_vbo->SetAttribute(tFaceShader::normal_attribute, GL_FLOAT);
	tang_vbo->SetAttribute(tFaceShader::tang_attribute, GL_FLOAT);
	bitang_vbo->SetAttribute(tFaceShader::bitang_attribute, GL_FLOAT);
	uvcoord_vbo->SetAttribute(tFaceShader::uvcoord_attribute, GL_FLOAT);
	tVAO::UnBind();

	refresh_vbos = true;
	refresh_ibos = true;
	idle_material = new tDefaultMaterial();
	material_ibos[idle_material] = new tMaterialIBO(this);

	if(file)
		LoadFromFile(file, material_manager);
}

tMesh::~tMesh(void)
{
	for(set<tMaterial *>::iterator mi=own_materials.begin(); mi!=own_materials.end(); mi++)
		delete *mi;

	for(map<tMaterial *, tMaterialIBO *>::iterator i=material_ibos.begin(); i!=material_ibos.end(); i++)
		delete i->second;

	for(vector<tEntity *>::iterator i=entities.begin(); i!=entities.end(); i++)
		delete *i;

	DeleteVBOData();

	delete physics_triangle_mesh;
	delete idle_material;
}


void tMesh::DeleteVBOData(void)
{
	if(vao)
	{
		delete vao;
		vao = 0;
		delete vertex_vbo;
		delete normal_vbo;
		delete tang_vbo;
		delete bitang_vbo;
		delete face_normal_vbo;
		delete uvcoord_vbo;
	}
}


tEntity *tMesh::CreateEntity(string name, string group)
{
	tEntity *e = new tEntity();
	e->name = name;
	e->group = group;
	entities.push_back(e);
	return e;
}

tMaterial *tMesh::GetMaterialByName(string name)
{
	map<string, tMaterial *>::iterator i = materials.find(name);

	if(i == materials.end())
		return 0;
	else
		return i->second;
}


tVertexIndex tMesh::AddVertex(tVertex v)
{
	vertices.push_back(v);
	refresh_vbos = true;
	return (tVertexIndex)(vertices.size() - 1);
}

void tMesh::AddTriangle(tTriangle t)
{
	triangles.push_back(t);
	refresh_vbos = true;
	refresh_ibos = true;
}

void tMesh::AddMaterial(string name, tMaterial *m)
{
	materials[name] = m;

	if(material_ibos.find(m) == material_ibos.end())
		material_ibos[m] = new tMaterialIBO(this);
}


/*void tMesh::RemoveVertex(tVertex *v)
{
	vector<tVertex *>::iterator i;

	for(i=vertices.begin(); i!=vertices.end(); i++)
		if(*i == v)
		{
			vertices.erase(i);
			break;
		}
	refresh_vbos = true;
	refresh_ibos = true;
}

void tMesh::RemoveTriangle(tTriangle *t)
{
	vector<tTriangle *>::iterator i;

	for(i=triangles.begin(); i!=triangles.end(); i++)
		if(*i == t)
		{
			triangles.erase(i);
			break;
		}
	refresh_ibos = true;
	//refresh_vbo = true;
}

void tMesh::RemoveMaterial(string name)
{
	map<string, tMaterial *>::iterator i = materials.find(name);

	if(i != materials.end())
	{
		map<tMaterial *, tMaterialIBO *>::iterator ii = material_ibos.find(i->second);
		if(ii != material_ibos.end())
		{
			delete ii->second;
			material_ibos.erase(ii);
		}

		materials.erase(i);
	}
}

void tMesh::RemoveEntity(tEntity *e)
{
	vector<tEntity *>::iterator i;

	for(i=entities.begin(); i!=entities.end(); i++)
		if(*i == e)
		{
			entities.erase(i);
			break;
		}
}*/


void tMesh::CalculateAllTangents()
{
	for(vector<tTriangle>::iterator i=triangles.begin(); i!=triangles.end(); i++)
		CalculateTangents(*i);
}

void tMesh::CalculateTangents(const tTriangle &t)
{
	tVertex &vert0 = vertices[t.v[0]];
	tVertex &vert1 = vertices[t.v[1]];
	tVertex &vert2 = vertices[t.v[2]];

	tVector edge1 = vert1.pos - vert0.pos;
	tVector edge2 = vert2.pos - vert0.pos;
	tVector2 uv_edge1 = vert1.uv - vert0.uv;
	tVector2 uv_edge2 = vert2.uv - vert0.uv;

	float r = uv_edge1.x * uv_edge2.y - uv_edge2.x * uv_edge1.y;

	tVector tang, bitang;
	if(r != 0.0)
	{
		r = 1.0f / r;

		tang = Vec(	uv_edge2.y * edge1.x - uv_edge1.y * edge2.x,
					   uv_edge2.y * edge1.y - uv_edge1.y * edge2.y,
					   uv_edge2.y * edge1.z - uv_edge1.y * edge2.z) * r;
		bitang = Vec(	uv_edge1.x * edge2.x - uv_edge2.x * edge1.x,
						 uv_edge1.x * edge2.y - uv_edge2.x * edge1.y,
						 uv_edge1.x * edge2.z - uv_edge2.x * edge1.z) * r;
	}
	else
	{
		tang = edge1;
		bitang = edge2;
	}

	tang.Normalize();
	bitang.Normalize();

	vert0.tang = vert1.tang = vert2.tang = tang;
	vert0.bitang = vert1.bitang = vert2.bitang = bitang;
}

void tMesh::GenerateBoundingBox(void)
{
	vector<tVertex>::iterator i;
	bounding_box = tBoundingBox();

	for(i=vertices.begin(); i!=vertices.end(); i++)
		bounding_box.AddPoint((*i).pos);
}

btTriangleMesh *tMesh::GeneratePhysicsMesh(void)
{
	vector<tTriangle>::iterator i;

	//if(physics_triangle_mesh)
	delete physics_triangle_mesh;

	if(triangles.empty())
		physics_triangle_mesh = 0;
	else
	{
		physics_triangle_mesh = new btTriangleMesh();

		for(i=triangles.begin(); i!=triangles.end(); i++)
			physics_triangle_mesh->addTriangle(BtVec(vertices[(*i).v[0]].pos),
											   BtVec(vertices[(*i).v[1]].pos),
											   BtVec(vertices[(*i).v[2]].pos));
	}

	return physics_triangle_mesh;
}


void tMesh::RefreshAllVBOs(void)
{
	int data_count = GetVertexCount();
	vertex_vbo->SetSize(data_count);
	normal_vbo->SetSize(data_count);
	tang_vbo->SetSize(data_count);
	bitang_vbo->SetSize(data_count);
	face_normal_vbo->SetSize(data_count);
	uvcoord_vbo->SetSize(data_count);

	float *vertex_data = vertex_vbo->GetData();
	float *normal_data = normal_vbo->GetData();
	float *tang_data = tang_vbo->GetData();
	float *bitang_data = bitang_vbo->GetData();
	float *face_normal_data = face_normal_vbo->GetData();
	float *uvcoord_data = uvcoord_vbo->GetData();

	for(int i=0; i<vertices.size(); i++)
	{
		tVertex &v = vertices[i];
		int d = i*3;
		//d = vt.index * 3;
		memcpy(vertex_data + d, v.pos.v, 3 * sizeof(float));
		memcpy(normal_data + d, v.normal.v, 3 * sizeof(float));
		memcpy(tang_data + d, v.tang.v, 3 * sizeof(float));
		memcpy(bitang_data + d, v.bitang.v, 3 * sizeof(float));
		memcpy(face_normal_data + d, v.normal.v, 3 * sizeof(float));
		memcpy(uvcoord_data + i*2, v.uv.v, 2 * sizeof(float));
	}

	vertex_vbo->AssignData();
	normal_vbo->AssignData();
	tang_vbo->AssignData();
	bitang_vbo->AssignData();
	face_normal_vbo->AssignData();
	uvcoord_vbo->AssignData();

	refresh_vbos = false;
}

void tMesh::RefreshIBOs(void)
{
	vector<tTriangle>::iterator i;
	tMaterial *mt;
	tMaterialIBO *mibo;
	int c;
	int j;

	SortMaterials();

	for(map<tMaterial *, tMaterialIBO *>::iterator m=material_ibos.begin(); m!=material_ibos.end(); m++)
		m->second->triangle_count = m->second->triangle_i = 0;

	for(i=triangles.begin(); i!=triangles.end(); i++)
	{
		mt = (*i).mat;
		if(!mt)
			mt = GetIdleMaterial();
		material_ibos[mt]->triangle_count++;
	}

	for(map<tMaterial *, tMaterialIBO *>::iterator m=material_ibos.begin(); m!=material_ibos.end(); m++)
	{
		mibo = m->second;
		mibo->ibo->SetSize(mibo->triangle_count * 3);
		mibo->ibo_data = mibo->ibo->GetData();
	}

	for(i=triangles.begin(), c=0; i!=triangles.end(); i++, c++)
	{
		tTriangle &t = *i;
		mt = t.mat;
		if(!mt)
			mt = GetIdleMaterial();
		mibo = material_ibos[mt];

		for(j=0; j<3; j++)
		{
			mibo->ibo_data[mibo->triangle_i * 3 + j] = t.v[j];
		}
		mibo->triangle_i++;
	}

	for(map<tMaterial *, tMaterialIBO *>::iterator m=material_ibos.begin(); m!=material_ibos.end(); m++)
		m->second->ibo->AssignData();

	refresh_ibos = false;
}


void tMesh::DepthPrePass(tRenderer *renderer, map<tMaterial *, tMaterial *> *replace_materials)
{
	if(refresh_vbos)
		RefreshAllVBOs();

	vao->Bind();

	if(refresh_ibos)
		RefreshIBOs();


	for(map<tMaterial *, tMaterialIBO *>::iterator i=material_ibos.begin(); i!=material_ibos.end(); i++)
	{
		tMaterial *mat = i->first;
		if(replace_materials)
		{
			map<tMaterial *, tMaterial *>::iterator ri = replace_materials->find(mat);
			if(ri != replace_materials->end())
				mat = ri->second;
		}

		if(mat->InitDepthPrePass(renderer))
			i->second->ibo->Draw(GL_TRIANGLES);
	}
}


void tMesh::GeometryPass(tRenderer *renderer, map<tMaterial *, tMaterial *> *replace_materials)
{
	if(refresh_vbos)
		RefreshAllVBOs();

	vao->Bind();

	if(refresh_ibos)
		RefreshIBOs();

	for(map<tMaterial *, tMaterialIBO *>::iterator i=material_ibos.begin(); i!=material_ibos.end(); i++)
	{
		tMaterial *mat = i->first;
		if(replace_materials)
		{
			map<tMaterial *, tMaterial *>::iterator ri = replace_materials->find(mat);
			if(ri != replace_materials->end())
				mat = ri->second;
		}

		if(mat->InitGeometryPass(renderer))
			i->second->ibo->Draw(GL_TRIANGLES);
	}
}

void tMesh::ForwardPass(tRenderer *renderer, float *transform, map<tMaterial *, tMaterial *> *replace_materials)
{
	bool vao_bound = false;

	for(map<tMaterial *, tMaterialIBO *>::iterator i=material_ibos.begin(); i!=material_ibos.end(); i++)
	{
		tMaterial *mat = i->first;
		if(replace_materials)
		{
			map<tMaterial *, tMaterial *>::iterator ri = replace_materials->find(mat);
			if(ri != replace_materials->end())
				mat = ri->second;
		}

		if(mat->InitForwardPass(renderer, transform))
		{
			if(!vao_bound)
			{
				vao_bound = true;
				vao->Bind();
				//current_pose->vbo->SetAttribute(tFaceShader::vertex_attribute, GL_FLOAT);
			}

			i->second->ibo->Draw(GL_TRIANGLES);
		}
	}
}


void tMesh::RefractionPass(tRenderer *renderer, float *transform, map<tMaterial *, tMaterial *> *replace_materials)
{
	bool vao_bound = false;

	for(map<tMaterial *, tMaterialIBO *>::iterator i=material_ibos.begin(); i!=material_ibos.end(); i++)
	{
		tMaterial *mat = i->first;
		if(replace_materials)
		{
			map<tMaterial *, tMaterial *>::iterator ri = replace_materials->find(mat);
			if(ri != replace_materials->end())
				mat = ri->second;
		}

		if(mat->InitRefractionPass(renderer, transform))
		{
			if(!vao_bound)
			{
				vao_bound = true;
				vao->Bind();
				//current_pose->vbo->SetAttribute(tFaceShader::vertex_attribute, GL_FLOAT);
			}

			i->second->ibo->Draw(GL_TRIANGLES);
		}
	}
}

bool tMesh::GetCubeMapReflectionEnabled(void)
{
	for(map<string, tMaterial *>::iterator i=materials.begin(); i!=materials.end(); i++)
		if(i->second->GetCubeMapReflectionEnabled())
			return true;

	return false;
}

bool tMesh::LoadFromFile(const char *file, tMaterialManager *material_manager)
{
	bool r;
	string path;
	
	path = PathOfFile(string(file));

	char *data = ReadFile(file);

	xml_document<> *doc = new xml_document<>();
	doc->parse<0>(data);
	r = LoadFromXML(doc, path, material_manager);

	delete doc;
	delete [] data;

	refresh_vbos = true;
	refresh_ibos = true;

	return r;
}

bool tMesh::LoadFromData(char *data, string path, tMaterialManager *material_manager)
{
	bool r;

	xml_document<> *doc = new xml_document<>();
	doc->parse<0>(data);

	r = LoadFromXML(doc, path, material_manager);

	refresh_vbos = true;
	refresh_ibos = true;

	return r;
}

int TEMVersionFromString(const char *s)
{
	if(strcmp(s, TEM_VERSION_0_4_STRING) == 0)
		return TEM_VERSION_0_4;
	if(strcmp(s, TEM_VERSION_0_3_STRING) == 0)
		return TEM_VERSION_0_3;
	if(strcmp(s, TEM_VERSION_0_2_STRING) == 0)
		return TEM_VERSION_0_2;
	if(strcmp(s, TEM_VERSION_0_1_STRING) == 0)
		return TEM_VERSION_0_1;
	return -1;
}

bool tMesh::LoadFromXML(xml_document<> *doc, string path, tMaterialManager *material_manager)
{
	xml_node<> *cur;
	xml_attribute<> *attr;

	if(!doc)
		return 0;

	cur = doc->first_node();
	if(!cur)
		return 0;

	if(strcmp(cur->name(), "tmesh") != 0)
		return 0;

	attr = cur->first_attribute("version");
	if(!attr)
	{
		printf("WARNING: File does not contain version info.\n");
	}
	else
	{
		int file_version = TEMVersionFromString(attr->value());
		if(file_version == -1)
		{
			printf("WARNING: Version of mesh file could not be recognized.\n");
		}
		else if(file_version != TEM_CURRENT_VERSION)
		{
			printf("WARNING: Version of mesh file of is outdated. Only %s is supported\n", TEM_CURRENT_VERSION_STRING);
		}
	}

	cur = cur->first_node();


	int current_vertex_id = 0;

	while(cur)
	{
		char *name = cur->name();

		if(strcmp(name, "vertex") == 0)
			ParseVertexNode(cur);
		else if(strcmp(name, "material") == 0)
			ParseMaterialNode(cur, path);
		else if(strcmp(name, "triangle") == 0)
			ParseTriangleNode(cur, material_manager);
		else if(strcmp(name, "mesh_data") == 0)
			ParseMeshDataNode(cur, material_manager, current_vertex_id);
		else if(strcmp(name, "entity") == 0)
			ParseEntityNode(cur);

		cur = cur->next_sibling();
	}

	CalculateAllTangents();

	GenerateBoundingBox();
	GeneratePhysicsMesh();

	refresh_vbos = true;
	refresh_ibos = true;

	return 1;
}

tVertexIndex tMesh::ParseVertexNode(xml_node<> *cur)
{
	int id;
	tVector p;
	tVector normal;
	tVector2 uv;
	tVertex *r;

	// TODO: check if attributes really are found

	p.x = (float)atof(cur->first_attribute("x")->value());
	p.y = (float)atof(cur->first_attribute("y")->value());
	p.z = (float)atof(cur->first_attribute("z")->value());

	uv.x = (float)atof(cur->first_attribute("u")->value());
	uv.y = (float)atof(cur->first_attribute("v")->value());
	normal.x = (float)atof(cur->first_attribute("nx")->value());
	normal.y = (float)atof(cur->first_attribute("ny")->value());
	normal.z = (float)atof(cur->first_attribute("nz")->value());


	tVertex v;
	v.pos = p;
	v.uv = uv;
	v.normal = normal;
	return AddVertex(v);
}



tMaterial *tMesh::ParseXMLMaterialNode(xml_node<> *cur, string &name, string path)
{
	xml_attribute<> *attr;

	if((attr = cur->first_attribute("type")))
	{
		if(strcmp(attr->value(), "simple_forward") == 0)
			return ParseXMLSimpleForwardMaterialNode(cur, name, path);
		else if(strcmp(attr->value(), "refraction") == 0)
			return ParseXMLRefractionMaterialNode(cur, name, path);
		else
			return ParseXMLDefaultMaterialNode(cur, name, path);
	}
	else
		return ParseXMLDefaultMaterialNode(cur, name, path);

}



void tMesh::LoadTextureFromXMLNodeData(xml_node<> *node, tDefaultMaterial *material, tDefaultMaterial::TextureType texture_type)
{
	unsigned char *data;
	size_t size;

	Base64Decode(node->value(), &data, &size);

	xml_attribute<> *attr = node->first_attribute("image-extension");
	char *extension;
	if(attr)
		extension = attr->value();
	else
		extension = 0;

	material->LoadTexture(texture_type, extension, data, size);
	delete [] data;
}

tDefaultMaterial *tMesh::ParseXMLDefaultMaterialNode(xml_node<> *cur, string &name, string path)
{
	xml_node<> *child;
	xml_attribute<> *attr;

	if(!(attr = cur->first_attribute("name")))
		return 0;

	name = string(attr->value());

	tDefaultMaterial *mat = new tDefaultMaterial();



	child = cur->first_node();
	while(child)
	{
		char *name_temp = child->name();

		if(strcmp(name_temp, "base_color") == 0)
		{
			if((attr = child->first_attribute("file")))
				mat->LoadTexture(tDefaultMaterial::BASE_COLOR, path + string(attr->value()));
			else if(child->value_size() > 0)
				LoadTextureFromXMLNodeData(child, mat, tDefaultMaterial::BASE_COLOR);

			tVector color = Vec(0.0, 0.0, 0.0);
			if((attr = child->first_attribute("r")))
				color.r = (float)atof(attr->value());
			if((attr = child->first_attribute("g")))
				color.g = (float)atof(attr->value());
			if((attr = child->first_attribute("b")))
				color.b = (float)atof(attr->value());
			mat->SetBaseColor(color);
		}
		else if(strcmp(name_temp, "metal_rough_reflect") == 0)
		{
			if((attr = child->first_attribute("file")))
				mat->LoadTexture(tDefaultMaterial::METAL_ROUGH_REFLECT, path + string(attr->value()));
			else if(child->value_size() > 0)
				LoadTextureFromXMLNodeData(child, mat, tDefaultMaterial::METAL_ROUGH_REFLECT);

			if((attr = child->first_attribute("metallic")))
				mat->SetMetallic((float)atof(attr->value()));
			if((attr = child->first_attribute("roughness")))
				mat->SetRoughness((float)atof(attr->value()));
			if((attr = child->first_attribute("reflectance")))
				mat->SetReflectance((float)atof(attr->value()));
		}
		else if(strcmp(name_temp, "normal") == 0)
		{
			if((attr = child->first_attribute("file")))
				mat->LoadTexture(tDefaultMaterial::NORMAL, path + string(attr->value()));
			else if(child->value_size() > 0)
				LoadTextureFromXMLNodeData(child, mat, tDefaultMaterial::NORMAL);
		}
		else if(strcmp(name_temp, "emission") == 0)
		{
			if((attr = child->first_attribute("file")))
				mat->LoadTexture(tDefaultMaterial::EMISSION, path + string(attr->value()));
			else if(child->value_size() > 0)
				LoadTextureFromXMLNodeData(child, mat, tDefaultMaterial::EMISSION);

			tVector color = Vec(0.0, 0.0, 0.0);
			if((attr = child->first_attribute("r")))
				color.r = (float)atof(attr->value());
			if((attr = child->first_attribute("g")))
				color.g = (float)atof(attr->value());
			if((attr = child->first_attribute("b")))
				color.b = (float)atof(attr->value());
			mat->SetEmission(color);
		}
		else if(strcmp(name_temp, "bump") == 0)
		{
			if((attr = child->first_attribute("file")))
				mat->LoadTexture(tDefaultMaterial::BUMP, path + string(attr->value()));
			else if(child->value_size() > 0)
				LoadTextureFromXMLNodeData(child, mat, tDefaultMaterial::BUMP);

			if((attr = child->first_attribute("depth")))
				mat->SetBumpDepth((float)atof(attr->value()));
		}
		else if(strcmp(name_temp, "shadow") == 0)
		{
			if((attr = child->first_attribute("cast")))
				mat->SetShadowCast(atoi(attr->value()) != 0);
		}

		child = child->next_sibling();
	}

	mat->UpdateUniformBuffer();

	return mat;
}


#define TEXTURE_DISABLED 0
#define TEXTURE_FILE 1
#define TEXTURE_DATA 2

tSimpleForwardMaterial *tMesh::ParseXMLSimpleForwardMaterialNode(xml_node<> *cur, string &name, string path)
{
	xml_node<> *child;

	int tex_mode = TEXTURE_DISABLED;
	string tex_file;
	tVector color = Vec(1.0, 1.0, 1.0);
	float alpha = 1.0;

	tSimpleForwardMaterial::BlendMode blend_mode = tSimpleForwardMaterial::ALPHA;

	unsigned char *tex_data = 0;
	size_t tex_size = 0;
	char *tex_ext = 0;
	char *base64_temp = 0;


	xml_attribute<> *attr;

	if(!(attr = cur->first_attribute("name")))
		return 0;

	name = string(attr->value());

	if((attr = cur->first_attribute("blend_mode")))
	{
		if(strcmp(attr->value(), "alpha") == 0)
			blend_mode = tSimpleForwardMaterial::ALPHA;
		else if(strcmp(attr->value(), "add") == 0)
			blend_mode = tSimpleForwardMaterial::ADD;
		else if(strcmp(attr->value(), "multiply") == 0)
			blend_mode = tSimpleForwardMaterial::MULTIPLY;
	}

	child = cur->first_node();
	while(child)
	{
		char *name_temp = child->name();

		if(strcmp(name_temp, "tex") == 0)
		{
			if((attr = child->first_attribute("file")))
			{
				tex_file = string(attr->value());
				tex_mode = TEXTURE_FILE;
			}
			else if(child->value_size() > 0)
			{
				base64_temp = child->value();

				Base64Decode(base64_temp, &tex_data, &tex_size);

				attr = child->first_attribute("image-extension");
				if(attr)
					tex_ext = attr->value();
				else
					tex_ext = 0;
				tex_mode = TEXTURE_DATA;
			}
		}
		else if(strcmp(name_temp, "color") == 0)
		{
			if((attr = child->first_attribute("r")))
				color.r = (float)atof(attr->value());
			if((attr = child->first_attribute("g")))
				color.g = (float)atof(attr->value());
			if((attr = child->first_attribute("b")))
				color.b = (float)atof(attr->value());
			if((attr = child->first_attribute("a")))
				alpha = (float)atof(attr->value());
		}

		child = child->next_sibling();
	}


	tSimpleForwardMaterial *r = new tSimpleForwardMaterial();

	r->SetColor(color, alpha);
	r->SetBlendMode(blend_mode);

	if(tex_mode == TEXTURE_FILE)
		r->LoadTexture(path + tex_file);
	else if(tex_mode == TEXTURE_DATA)
		r->LoadTexture(tex_ext, tex_data, tex_size);

	return r;
}


tRefractionMaterial *tMesh::ParseXMLRefractionMaterialNode(xml_node<> *cur, string &name, string path)
{
	xml_node<> *child;

	int color_tex_mode = TEXTURE_DISABLED;
	string color_tex_file;
	int normal_tex_mode = TEXTURE_DISABLED;
	string normal_tex_file;
	tVector color = Vec(1.0, 1.0, 1.0);

	tVector edge_color = Vec(0.0, 0.0, 0.0);
	float edge_alpha = 0.0;

	unsigned char *color_tex_data = 0;
	size_t color_tex_size = 0;
	char *color_tex_ext = 0;

	unsigned char *normal_tex_data = 0;
	size_t normal_tex_size = 0;
	char *normal_tex_ext = 0;

	char *base64_temp = 0;


	xml_attribute<> *attr;

	if(!(attr = cur->first_attribute("name")))
		return 0;

	name = string(attr->value());

	child = cur->first_node();
	while(child)
	{
		char *name_temp = child->name();

		if(strcmp(name_temp, "normal_tex") == 0)
		{
			if((attr = child->first_attribute("file")))
			{
				normal_tex_file = string(attr->value());
				normal_tex_mode = TEXTURE_FILE;
			}
			else if(child->value_size() > 0)
			{
				base64_temp = child->value();

				Base64Decode(base64_temp, &normal_tex_data, &normal_tex_size);

				attr = child->first_attribute("image-extension");
				if(attr)
					normal_tex_ext = attr->value();
				else
					normal_tex_ext = 0;
				normal_tex_mode = TEXTURE_DATA;
			}
		}
		else if(strcmp(name_temp, "color") == 0)
		{
			if((attr = child->first_attribute("r")))
				color.r = (float)atof(attr->value());
			if((attr = child->first_attribute("g")))
				color.g = (float)atof(attr->value());
			if((attr = child->first_attribute("b")))
				color.b = (float)atof(attr->value());


			if((attr = child->first_attribute("file")))
			{
				color_tex_file = string(attr->value());
				color_tex_mode = TEXTURE_FILE;
			}
			else if(child->value_size() > 0)
			{
				base64_temp = child->value();

				Base64Decode(base64_temp, &color_tex_data, &color_tex_size);

				attr = child->first_attribute("image-extension");
				if(attr)
					color_tex_ext = attr->value();
				else
					color_tex_ext = 0;
				color_tex_mode = TEXTURE_DATA;
			}
		}
		else if(strcmp(name_temp, "edge_color") == 0)
		{
			if((attr = child->first_attribute("r")))
				edge_color.r = (float)atof(attr->value());
			if((attr = child->first_attribute("g")))
				edge_color.g = (float)atof(attr->value());
			if((attr = child->first_attribute("b")))
				edge_color.b = (float)atof(attr->value());
			if((attr = child->first_attribute("a")))
				edge_alpha = (float)atof(attr->value());
		}

		child = child->next_sibling();
	}


	tRefractionMaterial *r = new tRefractionMaterial();

	r->SetColor(color);
	r->SetEdgeColor(edge_color, edge_alpha);

	if(color_tex_mode == TEXTURE_FILE)
		r->LoadColorTexture(path + color_tex_file);
	else if(color_tex_mode == TEXTURE_DATA)
		r->LoadColorTexture(color_tex_ext, color_tex_data, color_tex_size);

	if(normal_tex_mode == TEXTURE_FILE)
		r->LoadNormalTexture(path + normal_tex_file);
	else if(normal_tex_mode == TEXTURE_DATA)
		r->LoadNormalTexture(normal_tex_ext, normal_tex_data, normal_tex_size);

	return r;
}


tMaterial *tMesh::ParseMaterialNode(xml_node<> *cur, string path)
{
	string name;
	tMaterial *mat = ParseXMLMaterialNode(cur, name, path);

	if(mat)
	{
		AddMaterial(name, mat);
		own_materials.insert(mat);
	}

	return mat;
}

void tMesh::ParseMeshDataNode(xml_node<> *cur, tMaterialManager *material_manager, int &current_vertex_id)
{
	char *data = cur->value();
	size_t data_size = cur->value_size();

	//enum DataType { VERTEX, TRIANGLE };

	const int max_values_count = 9;

	//DataType current_data_type;

	char *values[max_values_count];

	int values_count = 0;
	size_t value_start = 0;
	for(size_t i=0; i<data_size; i++)
	{
		char &c = data[i];
		if(c == ' ')
		{
			values[values_count] = data + value_start;
			c = 0;
			values_count++;
			value_start = i+1;
		}

		if(values_count >= max_values_count)
		{
			while(true)
			{
				if(i >= data_size)
					break;
				if(data[i] == '\n' || data[i] == '\r')
					break;
			}
		}

		if(c == '\n' || c == '\r' || i == data_size-1)
		{
			if(values_count < max_values_count)
			{
				values[values_count] = data + value_start;
				if(c == '\n' || c == '\r')
					c = 0;
				values_count++;
			}

			/*printf("%d values: ", values_count);
			for(int j=0; j<values_count; j++)
			{
				printf("\"%s\" ", values[j]);
			}
			printf("\n");*/


			// process values
			if(values_count > 0)
			{
				if(strcmp(values[0], "v") == 0) // vertex
				{
					if(values_count >= 9)
					{
						tVector p;
						tVector normal;
						tVector2 uv;

						current_vertex_id++;
						p.x = (float)atof(values[1]);
						p.y = (float)atof(values[2]);
						p.z = (float)atof(values[3]);

						uv.x = (float)atof(values[4]);
						uv.y = (float)atof(values[5]);
						normal.x = (float)atof(values[6]);
						normal.y = (float)atof(values[7]);
						normal.z = (float)atof(values[8]);

						tVertex v;
						v.pos = p;
						v.uv = uv;
						v.normal = normal;
						AddVertex(v);
					}
				}
				else if(strcmp(values[0], "t") == 0)
				{
					if(values_count >= 5)
					{
						tMaterial *mat = 0;
						string mat_name = string(values[4]);
						map<string, tMaterial *>::iterator mi = materials.find(mat_name);
						if(mi == materials.end())
						{
							if(material_manager)
							{
								mat = material_manager->GetMaterial(mat_name);
								if(mat)
									AddMaterial(mat_name, mat);
							}
						}
						else
						{
							mat = mi->second;
						}

						if(!mat)
							mat = GetIdleMaterial();

						//CreateTriangle(GetVertexByID(atoi(values[1])), GetVertexByID(atoi(values[2])), GetVertexByID(atoi(values[3])), Vec(1.0, 1.0, 1.0), mat);
						tTriangle t;
						t.v[0] = (tVertexIndex)atoi(values[1]);
						t.v[1] = (tVertexIndex)atoi(values[2]);
						t.v[2] = (tVertexIndex)atoi(values[3]);
						t.mat = mat;
						AddTriangle(t);
					}
				}
			}



			// reset zero terminators
			for(int j=0; j<values_count; j++)
			{
				size_t len = strlen(values[j]);
				if(values[j] + len >= data + data_size)
					break;

				values[j][len] = j == values_count-1 ? '\n' : ' ';
			}

			values_count = 0;
			value_start = i+1;
		}
	}
}


void tMesh::ParseTriangleNode(xml_node<> *cur, tMaterialManager *material_manager)
{
	string mat_name;
	tVertexIndex v[3];
	int i;
	//tVertex *vt[3];
	//tTriangle *r;
	xml_node<> *child;
	xml_attribute<> *attr;


	if((attr = cur->first_attribute("mat")))
		mat_name = string(attr->value());

	child = cur->first_node("vertex");
	for(i=0; i<3 && child; child=child->next_sibling("vertex"))
	{
		attr = child->first_attribute("id");
		if(!attr)
			continue;

		v[i] = (tVertexIndex)atoi(attr->value());

		i++;
	}

	if(i<2) // less than 3 vertices
		return; // 0;

	tMaterial *mat = 0;
	map<string, tMaterial *>::iterator mi = materials.find(mat_name);
	if(mi == materials.end())
	{
		if(material_manager)
		{
			mat = material_manager->GetMaterial(mat_name);
			if(mat)
				AddMaterial(mat_name, mat);
		}
	}
	else
	{
		mat = mi->second;
	}

	if(!mat)
		mat = GetIdleMaterial();

	tTriangle t;
	t.v[0] = v[0];
	t.v[1] = v[1];
	t.v[2] = v[2];
	t.mat = mat;
	AddTriangle(t);
	//return t;
}


tEntity *tMesh::ParseEntityNode(xml_node<> *root)
{
	tEntity *e = new tEntity();
	tEntityAttribute *a;
	xml_node<> *cur;
	xml_attribute<> *attr;
	string a_type, a_name;


	if(!(attr = root->first_attribute("name")))
		return 0;

	e->name = string(attr->value());
	if(e->name.length() == 0)
	{
		delete e;
		return 0;
	}

	if((attr = root->first_attribute("group")))
		e->group = string(attr->value());

	for(cur=root->first_node(); cur; cur=cur->next_sibling())
	{
		a_type = string(cur->name());

		a = new tEntityAttribute();

		if(a_type.compare("vec3") == 0)
		{
			a->type = tEntityAttribute::VECTOR;

			tVector v;

			if((attr = cur->first_attribute("x")))
				v.x = (float)atof(attr->value());
			if((attr = cur->first_attribute("y")))
				v.y = (float)atof(attr->value());
			if((attr = cur->first_attribute("z")))
				v.z = (float)atof(attr->value());

			a->vec_v = v;
		}
		else if(a_type.compare("vec2") == 0)
		{
			a->type = tEntityAttribute::VECTOR2;

			tVector2 v;

			if((attr = cur->first_attribute("x")))
				v.x = (float)atof(attr->value());
			if((attr = cur->first_attribute("y")))
				v.y = (float)atof(attr->value());

			a->vec2_v = v;
		}
		else if(a_type.compare("float") == 0)
		{
			a->type = tEntityAttribute::FLOAT;

			float v = 0.0;

			if((attr = cur->first_attribute("v")))
				v = (float)atof(attr->value());

			a->float_v = v;
		}
		else if(a_type.compare("int") == 0)
		{
			a->type = tEntityAttribute::INT;

			int v = 0;

			if((attr = cur->first_attribute("v")))
				v = atoi(attr->value());

			a->int_v = v;
		}
		else if(a_type.compare("string") == 0)
		{
			a->type = tEntityAttribute::STRING;

			string v;

			if((attr = cur->first_attribute("v")))
				v = string(attr->value());

			a->string_v = v;
		}
		else
		{
			delete a;
			continue;
		}

		if(!(attr = cur->first_attribute("name")))
			continue;

		a_name = string(attr->value());

		e->attributes.insert(pair<string, tEntityAttribute *>(a_name, a));
	}

	entities.push_back(e);

	return e;
}



map<string, tEntity *> tMesh::GetEntitiesInGroup(const char *group)
{
	map<string, tEntity *> r;
	vector<tEntity *>::iterator i;

	for(i=entities.begin(); i!=entities.end(); i++)
		if((*i)->group.compare(group) == 0)
			r.insert(pair<string, tEntity *>((*i)->name, *i));

	return r;
}



/*bool CompareTriangleDist(tTriangle *a, tTriangle *b)
{
	return a->cam_dist > b->cam_dist;
}*/

/*bool CompareTriangleMaterial(tTriangle *a, tTriangle *b)
{
	//if(a->mat == b->mat)
	//	return CompareDist(a, b);
	//else
	if(b->mat->GetTransparent() == a->mat->GetTransparent())
		return a->mat < b->mat;
	else
		return b->mat->GetTransparent() && !a->mat->GetTransparent();
}*/

/*bool CompareMaterialTransparency(tMaterial *a, tMaterial *b)
{
	if(b->GetTransparent() == a->GetTransparent())
		return a < b;
	else
		return b->GetTransparent() && !a->GetTransparent();
}*/

/*void tMesh::SortTriangles(tVector cam)
{
	//vector<CTriangle *>::iterator t;
	//vector<CVertex *>::iterator v;

	//for(v=vertices.begin(); v!=vertices.end(); v++)
	//	(*v)->cam_dist = (cam - ApplyMatrix4(transformation, **v)).SquaredLen();
	
	//for(t=triangles.begin(); t!=triangles.end(); t++)
	//	(*t)->cam_dist = ((*t)->v[0]->cam_dist + (*t)->v[1]->cam_dist + (*t)->v[2]->cam_dist) * (1.0/3.0);

	sort(triangles.begin(), triangles.end(), CompareTriangleMaterial);
	refresh_vbos = true;
}*/

void tMesh::SortMaterials(void)
{
	return;
	//sort(materials.begin(), materials.end(), CompareMaterialTransparency);
}


