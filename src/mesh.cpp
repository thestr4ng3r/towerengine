#include "towerengine.h"

#define FILE_MODUS_MAT 0
#define FILE_MODUS_VERT 1
#define FILE_MODUS_TRI 2
#define FILE_MODUS_POS 3
#define FILE_MODUS_KEY 4
#define FILE_MODUS_ANIM 5
#define FILE_MODUS_END 10

#define FILE_MODUS_MAT_OLD 0
#define FILE_MODUS_TRI_OLD 1
#define FILE_MODUS_END_OLD 2

#define READ_MODUS_FINISH 1
#define READ_MODUS_READ -1
#define READ_MODUS_VALUES 0


tVector light_dir;
tVector light_pos;
tVector cam_pos;
tVector ambient_color;
tVector light_attenuation;





void tMesh::CalculateNormalsSolid(void)
{
	vector<tTriangle *>::iterator i;
	for(i=triangles.begin(); i!=triangles.end(); i++)
		(*i)->CalculateNormalSolid();
}

tMesh::tMesh(const char *file)
{
	idle_pose = 0;
	vao = 0;
	physics_triangle_mesh = 0;
	bounding_box = tBoundingBox();
	//mat_indices = 0;
	refresh_func = 0;
	refresh_vbos = true;
	refresh_ibos = true;
	outdated_vertices.clear();
	vertex_indices.clear();
	entities.clear();
	Create();

	if(file)
		LoadFromFile(file);
}

tMesh::~tMesh(void)
{
	Delete();
}

float tMesh::color[4] = { 1.0, 1.0, 1.0, 1.0 };


void tMesh::Color(tVector c, float a)
{
	color[0] = c.x;
	color[1] = c.y;
	color[2] = c.z;
	color[3] = a;
}


void tMesh::ApplyMatrix(float m[16])
{
	vector<tVertex *>::iterator v;

	for(v=vertices.begin(); v!=vertices.end(); v++)
		(*v)->SetVector(ApplyMatrix4(m, **v));
}


void tMesh::Create(void)
{
	Delete();

	vao = new tVAO();
	current_pose = idle_pose = new tMeshPose(this);
	//vertex_vbo = new VBO<float>(3, vao);
	normal_vbo = new tVBO<float>(3, vao);
	tang_vbo = new tVBO<float>(3, vao);
	bitang_vbo = new tVBO<float>(3, vao);
	face_normal_vbo = new tVBO<float>(3, vao);
	uvcoord_vbo = new tVBO<float>(2, vao);
	outdated_vertices.clear();
	data_count = 0;
	refresh_func = 0;
	refresh_vbos = true;
	refresh_ibos = true;
	idle_material = new tMeshMaterial(this, string());
	//VAO::UnBind();

	SetWireframe(0);
	SetAnimationLoop(1);
	ResetAnimationFinished();
}

void tMesh::Delete(void)
{
	if(!GetState())
		return;

	map<string, tMeshPose *>::iterator cpi;
	for(cpi=custom_pose.begin(); cpi!=custom_pose.end(); cpi++)
		delete custom_pose.begin()->second;
	custom_pose.clear();

	while(!animations.empty())
		delete animations.back();

	if(idle_pose)
		delete idle_pose;
	
	while(!triangles.empty())
		delete triangles.back();
	
	while(!materials.empty())
		delete materials.back();
	
	while(!vertices.empty())
		delete vertices.back();

	while(!entities.empty())
	{
		delete entities.back();
		entities.pop_back();
	}

	vertex_indices.clear();

	DeleteVBOData();
	outdated_vertices.clear();
	refresh_vbos = true;
	refresh_ibos = true;

	if(physics_triangle_mesh)
	{
		delete physics_triangle_mesh;
		physics_triangle_mesh = 0;
	}


	data_count = 0;
}

void tMesh::DeleteVBOData(void)
{
	if(vao)
	{
		delete vao;
		vao = 0;
		//delete vertex_vbo;
		delete normal_vbo;
		delete tang_vbo;
		delete bitang_vbo;
		delete face_normal_vbo;
		delete uvcoord_vbo;
	}
}

tVBO<float> *tMesh::CreateFloatVBO(int components)
{
	return new tVBO<float>(components, vao);
}


tVertex *tMesh::CreateVertex(tVector v)
{
	tVertex *o;
	o = new tVertex(v, this);
	refresh_vbos = false;
	return o;
}

tTriangle *tMesh::CreateTriangle(tVertex *v1, tVertex *v2, tVertex *v3, tVector color, char material[100], tVector t1, tVector t2, tVector t3) // TODO: Optimieren!
{
    if(!GetState())
        return 0;

	refresh_ibos = true;

    return tTriangle::CreateTriangle(v1, v2, v3, color, material, t1, t2, t3, this);
}

tTriangle *tMesh::CreateTriangleAuto(tVector v1, tVector v2, tVector v3, tVector color, char material[100], tVector t1, tVector t2, tVector t3)
{
	tVertex *vert[3];
	vector<tVertex *>::iterator verti;
	int i;
	float v[9] = {v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z};

	vert[0] = vert[1] = vert[2] = 0;
	for(i=0; i<3; i++)
	{
		for(verti=vertices.begin(); verti!=vertices.end(); verti++)
			if((*verti)->x == v[i*3+0] && (*verti)->y == v[i*3+1] && (*verti)->z == v[i*3+2])
			{
				vert[i] = *verti;
				break;
			}
		if(!vert[i])
			vert[i] = CreateVertex(Vec(v[i*3+0], v[i*3+1], v[i*3+2]));
	}

	refresh_ibos = true;

	return CreateTriangle(vert[0], vert[1], vert[2],
               color,			     // color
               material,                    // material
               t1,			    // t1
               t2,			    // t2
               t3);			    // t3
}

tEntity *tMesh::CreateEntity(string name, string group)
{
	tEntity *e = new tEntity();
	e->name = name;
	e->group = group;
	entities.push_back(e);
	return e;
}

tMeshMaterial *tMesh::GetMaterialByName(string name)
{
    vector<tMeshMaterial *>::iterator m;

    for(m=materials.begin(); m!=materials.end(); m++)
    {
        if((*m)->GetName().compare(name) == 0)
            return *m;
    }

    return 0;
}

tVertex *tMesh::GetVertexByID(int id)
{
	map<int, tVertex *>::iterator i = vertex_indices.find(id);
	if(i != vertex_indices.end())
		return i->second;

	return 0;

	/*
	vector<CVertex *>::iterator v;
	int i = 0;

	for(v=vertices.begin(); v!=vertices.end(); v++)
		if((*v)->id == id)
		{
			if(i>=w)
				return *v;
			else i++;
		}

	return 0;*/
}

void tMesh::SetVertexId(tVertex *v, int id)
{
	if(v->id != 0)
		vertex_indices.erase(v->id);

	v->id = id;
	vertex_indices.insert(pair<int, tVertex *>(id, v));
}

void tMesh::AddVertex(tVertex *v)
{
	vector<tVertex *>::iterator i;

	for(i=vertices.begin(); i!=vertices.end(); i++)
		if(*i == v)
			return;
	vertices.push_back(v);

	refresh_vbos = true;
}

void tMesh::AddTriangle(tTriangle *t)
{
	vector<tTriangle *>::iterator i;

	for(i=triangles.begin(); i!=triangles.end(); i++)
		if(*i == t)
			return;
	triangles.push_back(t);
	refresh_vbos = true;
	refresh_ibos = true;
}

void tMesh::AddMaterial(tMeshMaterial *m)
{
	vector<tMeshMaterial *>::iterator i;

	for(i=materials.begin(); i!=materials.end(); i++)
		if(*i == m)
			return;
	materials.push_back(m);
	refresh_ibos = true;
}

void tMesh::AddCustomPose(string name, tMeshPose *p)
{
	if(custom_pose.find(name) != custom_pose.end())
		return;

	custom_pose.insert(pair<string, tMeshPose *>(name, p));
	//refresh_vbo = true;
}

void tMesh::AddAnimation(tAnimation *a)
{
	vector<tAnimation *>::iterator i;

	for(i=animations.begin(); i!=animations.end(); i++)
		if(*i == a)
			return;
	animations.push_back(a);
	//refresh_vbo = true;
}

void tMesh::RemoveVertex(tVertex *v)
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

void tMesh::RemoveMaterial(tMeshMaterial *m)
{
	vector<tMeshMaterial *>::iterator i;

	for(i=materials.begin(); i!=materials.end(); i++)
		if(*i == m)
		{
			materials.erase(i);
			break;
		}
	refresh_ibos = true;
	//refresh_vbo = true;
}

void tMesh::RemoveCustomPose(string name)
{
	map<string, tMeshPose *>::iterator i;

	i = custom_pose.find(name);

	if(i != custom_pose.end())
		custom_pose.erase(i);
}

void tMesh::RemoveAnimation(tAnimation *a)
{
	vector<tAnimation *>::iterator i;

	for(i=animations.begin(); i!=animations.end(); i++)
		if(*i == a)
		{
			animations.erase(i);
			break;
		}
	//refresh_vbo = true;
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
}

void tMesh::SetIDs(void)
{
	int i;
	vector<tVertex *>::iterator v;
	vector<tAnimation *>::iterator a;
	tKeyFrame *f;
	map<string, tMeshPose *>::iterator p;

	for(v=vertices.begin(), i=0; v!=vertices.end(); v++, i++)
		SetVertexId((*v), i);

	for(a=animations.begin(); a!=animations.end(); a++)
		for(f=(*a)->key_first; f; f=f->chain_next, i++)
			f->id = i;

	for(p=custom_pose.begin(), i=0; p!=custom_pose.end(); p++)
		p->second->id = i;
}

void tMesh::SetTriangleMaterials(void)
{
    vector<tTriangle *>::iterator t;

    for(t=triangles.begin(); t!=triangles.end(); t++)
    {
        (*t)->mat = GetMaterialByName((*t)->m_name);
        if(!(*t)->mat)
        	(*t)->mat = GetIdleMaterial();
    }
	//refresh_vbo = true;
}

void tMesh::AssignVertexArrayPositions(void)
{
	vector<tVertex *>::iterator i;
	int c;

	for(c=0, i=vertices.begin(); i!=vertices.end(); c++, i++)
		(*i)->index = c;

	refresh_vbos = true;
}

void tMesh::GenerateBoundingBox(void)
{
	vector<tVertex *>::iterator i;
	bounding_box = tBoundingBox();

	for(i=vertices.begin(); i!=vertices.end(); i++)
		bounding_box.AddPoint(**i);
}

btTriangleMesh *tMesh::GeneratePhysicsMesh(void)
{
	vector<tTriangle *>::iterator i;

	if(physics_triangle_mesh)
		delete physics_triangle_mesh;

	physics_triangle_mesh = new btTriangleMesh();

	for(i=triangles.begin(); i!=triangles.end(); i++)
		physics_triangle_mesh->addTriangle(BtVec(*((*i)->v[0])), BtVec(*((*i)->v[1])), BtVec(*((*i)->v[2])));

	return physics_triangle_mesh;
}


void tMesh::RefreshAllVBOs(void)
{
	vector<tTriangle *>::iterator i;
	vector<tMeshMaterial *>::iterator m;
	vector<tVertex *>::iterator v;
	tVertex *vt;
	int d;
	float *normal_data;
	float *tang_data;
	float *bitang_data;
	float *face_normal_data;
	float *uvcoord_data;

	AssignVertexArrayPositions();

	data_count = GetVertexCount();
	normal_vbo->SetSize(data_count);
	tang_vbo->SetSize(data_count);
	bitang_vbo->SetSize(data_count);
	face_normal_vbo->SetSize(data_count);
	uvcoord_vbo->SetSize(data_count);

	normal_data = normal_vbo->GetData();
	tang_data = tang_vbo->GetData();
	bitang_data = bitang_vbo->GetData();
	face_normal_data = face_normal_vbo->GetData();
	uvcoord_data = uvcoord_vbo->GetData();

	for(v=vertices.begin(); v!=vertices.end(); v++)
	{
		vt = *v;
		d = vt->index * 3;
		memcpy(normal_data + d, vt->normal.v, 3 * sizeof(float));
		memcpy(tang_data + d, vt->tang.v, 3 * sizeof(float));
		memcpy(bitang_data + d, vt->bitang.v, 3 * sizeof(float));
		memcpy(face_normal_data + d, vt->normal.v, 3 * sizeof(float));
		memcpy(uvcoord_data + vt->index*2, vt->uv.v, 2 * sizeof(float));

	}

	normal_vbo->AssignData();
	tang_vbo->AssignData();
	bitang_vbo->AssignData();
	face_normal_vbo->AssignData();
	uvcoord_vbo->AssignData();

	refresh_vbos = false;
}

void tMesh::RefreshIBOs(void)
{
	vector<tTriangle *>::iterator i;
	vector<tMeshMaterial *>::iterator m;
	tTriangle *t;
	tMeshMaterial *mt;
	int c;
	int j;

	SortMaterials();

	for(m=materials.begin(); m!=materials.end(); m++)
		(*m)->triangle_count = (*m)->triangle_i = 0;

	for(i=triangles.begin(); i!=triangles.end(); i++)
	{
		(*i)->mat->triangle_count++;
	}

	for(m=materials.begin(); m!=materials.end(); m++)
	{
		mt = *m;
		mt->ibo->SetSize(mt->triangle_count * 3);
		mt->ibo_data = mt->ibo->GetData();
	}

	for(i=triangles.begin(), c=0; i!=triangles.end(); i++, c++)
	{
		t = *i;

		mt = t->mat;
		for(j=0; j<3; j++)
			mt->ibo_data[mt->triangle_i*3 + j] = t->v[j]->index;
		mt->triangle_i++;
	}

	for(m=materials.begin(); m!=materials.end(); m++)
		(*m)->ibo->AssignData();

	refresh_ibos = false;
}

void tMesh::PutToGL(void)
{
	vector<tMeshMaterial *>::iterator i;
	tVBO<float> *vertex_vbo, *vertex2_vbo;
	tKeyFrame *a, *b;
	float mix;

	vertex2_vbo = 0;
	if(animation_mode)
	{
		current_animation->GetKeyframePair(&a, &b, &mix);
		if(!a)
		{
			vertex_vbo = current_pose->vbo;
		}
		else
		{
			vertex_vbo = a->vbo;
			if(b)
				vertex2_vbo = b->vbo;
		}
	}
	else
	{
		vertex_vbo = current_pose->vbo;
	}

	//Sort(cam);

	if(refresh_vbos)
		RefreshAllVBOs();


/*	glColor4f(1.0, 1.0, 1.0, 1.0);
	vector<CTriangle *>::iterator ti;
	for(ti=triangles.begin(); ti!=triangles.end(); ti++)
	{
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1.0, 1.0, 1.0);
		for(int j=0; j<3; j++)
		{
			(*ti)->v[j]->PutToGL();
			(*((*ti)->v[j]) + (*ti)->v[j]->normal * 0.1).PutToGL();
		}
		glEnd();
	}*/

	vertex_vbo->SetAttribute(tFaceShader::vertex_attribute, GL_FLOAT);
	if(vertex2_vbo)
	{
		tEngine::GetCurrentFaceShader()->SetVertexMix(mix);
		vertex2_vbo->SetAttribute(tFaceShader::vertex2_attribute, GL_FLOAT);
	}
	else
		tEngine::GetCurrentFaceShader()->SetVertexMix(0.0);

	normal_vbo->SetAttribute(tFaceShader::normal_attribute, GL_FLOAT);
	tang_vbo->SetAttribute(tFaceShader::tang_attribute, GL_FLOAT);
	bitang_vbo->SetAttribute(tFaceShader::bitang_attribute, GL_FLOAT);
	uvcoord_vbo->SetAttribute(tFaceShader::uvcoord_attribute, GL_FLOAT);

	if(refresh_ibos)
		RefreshIBOs();

	//CEngine::GetFaceShader()->BindShader();
	tEngine::GetCurrentFaceShader()->SetDiffuseColor2(Vec(color[0], color[1], color[2]), color[3]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for(i=materials.begin(); i!=materials.end(); i++)
	{
		(*i)->PutToGL();

		if(this->wireframe)
			(*i)->ibo->Draw(GL_LINE_STRIP);
		else
			(*i)->ibo->Draw(GL_TRIANGLES);
	}
}

int tMesh::GetState(void)
{
    return 1;
}

bool tMesh::LoadFromFile(const char *file, int no_material)
{
	bool r;
	char *path;
	struct stat s;
	
	if(stat(file, &s) != 0 && errno == ENOENT)
	    return 0;
	
	path = PathOfFile(file);

	r = LoadFromXML(xmlParseFile(file), path, no_material);

	refresh_vbos = true;
	refresh_ibos = true;

	return r;
}

bool tMesh::LoadFromData(const char *data, const char *path)
{
	bool r;

	r = LoadFromXML(xmlParseDoc((const xmlChar *)data), path, 0);

	refresh_vbos = true;
	refresh_ibos = true;

	return r;
}

int TEMVersionFromString(const char *s)
{
	if(strcmp(s, TEM_VERSION_0_3_STRING) == 0)
		return TEM_VERSION_0_3;
	if(strcmp(s, TEM_VERSION_0_2_STRING) == 0)
		return TEM_VERSION_0_2;
	if(strcmp(s, TEM_VERSION_0_1_STRING) == 0)
		return TEM_VERSION_0_1;
	return -1;
}

bool tMesh::LoadFromXML(xmlDocPtr doc, const char *path, int no_material)
{
	xmlNodePtr cur;
	char *version_string;

	if(!doc)
		return 0;

	cur = xmlDocGetRootElement(doc);
	if(!cur)
		return 0;

	if(xmlStrcmp(cur->name, (const xmlChar *)"tmesh"))
		return 0;

	version_string = (char *)xmlGetProp(cur, (const xmlChar *)"version");
	if(!version_string)
	{
		printf("WARNING: File does not contain version info. Assuming it is %s.\n", TEM_VERSION_0_1_STRING);
		file_version = TEM_VERSION_0_1;
	}
	else
	{
		file_version = TEMVersionFromString(version_string);
		if(file_version == -1)
		{
			printf("WARNING: Version \"%s\" of could not be recognized. Assuming it is %s.\n",
					version_string, TEM_CURRENT_VERSION_STRING);
			file_version = TEM_CURRENT_VERSION;
		}
		else if(file_version != TEM_CURRENT_VERSION)
		{
			printf("WARNING: Version \"%s\" of is outdated. Use \"%s\" instead.\n",
					version_string, TEM_CURRENT_VERSION_STRING);
		}
	}

	Create();

	cur = cur->children;

	while(cur)
	{
		if(xmlStrcmp(cur->name, (const xmlChar *)"vertex") == 0)
			ParseVertexNode(cur);
		if(xmlStrcmp(cur->name, (const xmlChar *)"material") == 0 && !no_material)
			ParseMaterialNode(cur, path);
		if(xmlStrcmp(cur->name, (const xmlChar *)"triangle") == 0)
			ParseTriangleNode(cur);
		if(xmlStrcmp(cur->name, (const xmlChar *)"pose") == 0 || xmlStrcmp(cur->name, (const xmlChar *)"position") == 0)
			ParsePoseNode(cur);
		if(xmlStrcmp(cur->name, (const xmlChar *)"animation") == 0)
			ParseAnimationNode(cur);
		if(xmlStrcmp(cur->name, (const xmlChar *)"entity") == 0)
			ParseEntityNode(cur);


		//CallRefresh();

		cur = cur->next;
	}


	idle_pose->CopyFromVertices();

	GenerateBoundingBox();
	GeneratePhysicsMesh();

	refresh_vbos = true;
	refresh_ibos = true;

	return 1;
}

tVertex *tMesh::ParseVertexNode(xmlNodePtr cur)
{
	int id;
	tVector p;
	tVector normal;
	tVector2 uv;
	tVertex *r;

	id = atoi((const char *)xmlGetProp(cur, (const xmlChar *)"id"));
	p.x = atof((const char *)xmlGetProp(cur, (const xmlChar *)"x"));
	p.y = atof((const char *)xmlGetProp(cur, (const xmlChar *)"y"));
	p.z = atof((const char *)xmlGetProp(cur, (const xmlChar *)"z"));

	if(file_version >= TEM_VERSION_0_2)
	{
		uv.x = atof((const char *)xmlGetProp(cur, (const xmlChar *)"u"));
		uv.y = atof((const char *)xmlGetProp(cur, (const xmlChar *)"v"));
		normal.x = atof((const char *)xmlGetProp(cur, (const xmlChar *)"nx"));
		normal.y = atof((const char *)xmlGetProp(cur, (const xmlChar *)"ny"));
		normal.z = atof((const char *)xmlGetProp(cur, (const xmlChar *)"nz"));
	}

	r = new tVertex(p, this);
	SetVertexId(r, id);
	r->uv_set = file_version >= TEM_VERSION_0_2;
	r->uv = uv;
	r->normal = normal;
	return r;
}

#define TEXTURE_DISABLED 0
#define TEXTURE_FILE 1
#define TEXTURE_DATA 2

tMeshMaterial *tMesh::ParseMaterialNode(xmlNodePtr cur, const char *path)
{
	xmlNodePtr child;
	int diffuse_mode, specular_mode, normal_mode, bump_mode;
	string diffuse_file; tVector diffuse_color;
	string specular_file; float exponent; tVector specular_color;
	string normal_file;
	string bump_file; float bump_depth;

	unsigned char *diffuse_data, *specular_data, *normal_data, *bump_data;
	size_t diffuse_size, specular_size, normal_size, bump_size;
	char *diffuse_ext, *specular_ext, *normal_ext, *bump_ext;
	char *base64_temp;

	string name;
	xmlChar *temp;
	tMeshMaterial *r;

	diffuse_mode = specular_mode = normal_mode = bump_mode = TEXTURE_DISABLED;
	diffuse_ext = specular_ext = normal_ext = bump_ext = 0;
	diffuse_size = specular_size = normal_size = bump_size = 0;
	diffuse_data = specular_data = normal_data = bump_data = 0;
	diffuse_color = Vec(1.0, 1.0, 1.0);
	specular_color = Vec(0.0, 0.0, 0.0);
	exponent = 8.0;
	bump_depth = 0.0;

	name = string((const char *)xmlGetProp(cur, (const xmlChar *)"name"));

	child = cur->children;
	while(child)
	{
		if(!xmlStrcmp(child->name, (const xmlChar *)"diffuse"))
		{
			if((temp = xmlGetProp(child, (const xmlChar *)"file")))
			{
				diffuse_file = string((const char *)temp);
				diffuse_mode = TEXTURE_FILE;
			}
			else if((base64_temp = (char *)xmlNodeListGetString(child->doc, child->children, 1)))
			{
				Base64Decode(base64_temp, &diffuse_data, &diffuse_size);
				if((temp = xmlGetProp(child, (const xmlChar *)"image-extension")))
					diffuse_ext = (char *)temp;
				else
					diffuse_ext = 0;
				diffuse_mode = TEXTURE_DATA;
			}

			if((temp = xmlGetProp(child, (const xmlChar *)"r")))
				diffuse_color.r = atof((const char *)temp);
			if((temp = xmlGetProp(child, (const xmlChar *)"g")))
				diffuse_color.g = atof((const char *)temp);
			if((temp = xmlGetProp(child, (const xmlChar *)"b")))
				diffuse_color.b = atof((const char *)temp);
		}
		if(!xmlStrcmp(child->name, (const xmlChar *)"specular"))
		{
			if((temp = xmlGetProp(child, (const xmlChar *)"file")))
			{
				specular_file = string((const char *)temp);
				specular_mode = TEXTURE_FILE;
			}
			else if((base64_temp = (char *)xmlNodeListGetString(child->doc, child->children, 1)))
			{
				Base64Decode(base64_temp, &specular_data, &specular_size);
				if((temp = xmlGetProp(child, (const xmlChar *)"image-extension")))
					specular_ext = (char *)temp;
				else
					specular_ext = 0;
				specular_mode = TEXTURE_DATA;
			}
			if((temp = xmlGetProp(child, (const xmlChar *)"exponent")))
				exponent = atof((const char *)temp);
			if((temp = xmlGetProp(child, (const xmlChar *)"r")))
				specular_color.r = atof((const char *)temp);
			if((temp = xmlGetProp(child, (const xmlChar *)"g")))
				specular_color.g = atof((const char *)temp);
			if((temp = xmlGetProp(child, (const xmlChar *)"b")))
				specular_color.b = atof((const char *)temp);
		}
		if(!xmlStrcmp(child->name, (const xmlChar *)"normal"))
		{
			if((temp = xmlGetProp(child, (const xmlChar *)"file")))
			{
				normal_file = string((const char *)temp);
				normal_mode = TEXTURE_FILE;
			}
			else if((base64_temp = (char *)xmlNodeListGetString(child->doc, child->children, 1)))
			{
				Base64Decode(base64_temp, &normal_data, &normal_size);
				if((temp = xmlGetProp(child, (const xmlChar *)"image-extension")))
					normal_ext = (char *)temp;
				else
					normal_ext = 0;
				normal_mode = TEXTURE_DATA;
			}

		}
		if(!xmlStrcmp(child->name, (const xmlChar *)"bump"))
		{
			if((temp = xmlGetProp(child, (const xmlChar *)"file")))
			{
				bump_file = string((const char *)temp);
				bump_mode = TEXTURE_FILE;
			}
			else if((base64_temp = (char *)xmlNodeListGetString(child->doc, child->children, 1)))
			{
				Base64Decode(base64_temp, &bump_data, &bump_size);
				if((temp = xmlGetProp(child, (const xmlChar *)"image-extension")))
					bump_ext = (char *)temp;
				else
					bump_ext = 0;
				bump_mode = TEXTURE_DATA;
			}
			if((temp = xmlGetProp(child, (const xmlChar *)"depth")))
				bump_depth = atof((const char *)temp);
		}

		child = child->next;
	}

	r = new tMeshMaterial(this, name);


	r->SetDiffuse(diffuse_color);
	r->SetSpecular(specular_color, exponent);
	r->SetBump(bump_depth);


	if(diffuse_mode == TEXTURE_FILE)
		r->LoadDiffuseTextureURL(string(path) + diffuse_file);
	else if(diffuse_mode == TEXTURE_DATA)
		r->LoadDiffuseTextureBinary(diffuse_ext, diffuse_data, diffuse_size);

	if(specular_mode == TEXTURE_FILE)
		r->LoadSpecularTextureURL(string(path) + specular_file);
	else if(specular_mode == TEXTURE_DATA)
		r->LoadSpecularTextureBinary(specular_ext, specular_data, specular_size);

	if(normal_mode == TEXTURE_FILE)
		r->LoadNormalTextureURL(string(path) + normal_file);
	else if(normal_mode == TEXTURE_DATA)
		r->LoadNormalTextureBinary(normal_ext, normal_data, normal_size);

	if(bump_mode == TEXTURE_FILE)
		r->LoadBumpTextureURL(string(path) + bump_file);
	else if(bump_mode == TEXTURE_DATA)
		r->LoadBumpTextureBinary(bump_ext, bump_data, bump_size);

	return r;
}

tTriangle *tMesh::ParseTriangleNode(xmlNodePtr cur)
{
	char *material;
	int v[3];
	int i;
	tVector uv[3];
	tVertex *vt[3];
	tTriangle *r;
	xmlNodePtr child;

	material = (char *)xmlGetProp(cur, (const xmlChar *)"mat");

	child = cur->children;
	i = 0;
	for(i=0; i<3 && child; child=child->next)
	{
		if(xmlStrcmp(child->name, (const xmlChar *)"vertex"))
			continue;

		v[i] = atoi((const char *)xmlGetProp(child, (const xmlChar *)"id"));
		vt[i] = GetVertexByID(v[i]);

		if(file_version < TEM_VERSION_0_2)
		{
			uv[i].x = atof((const char *)xmlGetProp(child, (const xmlChar *)"u"));
			uv[i].y = atof((const char *)xmlGetProp(child, (const xmlChar *)"v"));
			uv[i].z = 0.0;

			if(!vt[i]->uv_set)
			{
				vt[i]->uv = uv[i].xy();
				vt[i]->uv_set = true;
			}
			else if(vt[i]->uv.x != uv[i].x || vt[i]->uv.y != uv[i].y)
			{
				vt[i] = CreateVertex(*vt[i]);
				vt[i]->uv = uv[i].xy();
				SetVertexId(vt[i], v[i]);
			}
		}
		i++;
	}
	if(i<2)
	{
		printf("Warning: Triangle has less than 3 Vertices! Ignoring...\n");
		return 0;
	}

	r = CreateTriangle(vt[0], vt[1], vt[2], Vec(1.0, 1.0, 1.0), material, uv[0], uv[1], uv[2]);
	return r;
}

tMeshPose *tMesh::ParsePoseNode(xmlNodePtr cur)
{
	char *name;
	xmlNodePtr child;
	int count;
	int i;
	int *vert;
	tVector *vec;
	tMeshPose *r;

	name = (char *)xmlGetProp(cur, (const xmlChar *)"name");

	child = cur->children;
	count = 0;
	while(child)
	{
		if(xmlStrcmp(child->name, (const xmlChar *)"vertex") == 0)
			count++;
		child = child->next;
	}

	vert = new int[count];
	vec = new tVector[count];

	child = cur->children;
	for(child = cur->children, i=0; child && i<count; child = child->next)
	{
		if(xmlStrcmp(child->name, (const xmlChar *)"vertex"))
			continue;

		vert[i] = atoi((char *)xmlGetProp(child, (const xmlChar *)"id"));
		vec[i].x = atof((char *)xmlGetProp(child, (const xmlChar *)"x"));
		vec[i].y = atof((char *)xmlGetProp(child, (const xmlChar *)"y"));
		vec[i].z = atof((char *)xmlGetProp(child, (const xmlChar *)"z"));

		i++;
	}

	r = new tMeshPose(this);
	r->CopyFromData(count, vert, vec);
	AddCustomPose(name, r);
	return r;
}

tAnimation *tMesh::ParseAnimationNode(xmlNodePtr cur)
{
	char *name;
	float len;
	xmlNodePtr child;
	tAnimation *r;

	name = (char *)xmlGetProp(cur, (const xmlChar *)"name");
	len = atof((char *)xmlGetProp(cur, (const xmlChar *)"len"));

	r = new tAnimation(this, name, len, 0);

	for(child = cur->children; child; child = child->next)
	{
		if(xmlStrcmp(child->name, (const xmlChar *)"frame"))
			continue;
		ParseKeyFrameNode(child, r);
	}

	return r;
}

tKeyFrame *tMesh::ParseKeyFrameNode(xmlNodePtr cur, tAnimation *anim)
{
	float time;
	xmlNodePtr child;
	int count;
	int i;
	int *vert;
	tVector *vec;
	tKeyFrame *r;

	time = atof((char *)xmlGetProp(cur, (const xmlChar *)"time"));

	child = cur->children;
	count = 0;
	while(child)
	{
		if(xmlStrcmp(child->name, (const xmlChar *)"vertex") == 0)
			count++;
		child = child->next;
	}

	vert = new int[count];
	vec = new tVector[count];


	child = cur->children;
	for(child = cur->children, i=0; child && i<count; child = child->next)
	{
		if(xmlStrcmp(child->name, (const xmlChar *)"vertex"))
			continue;

		vert[i] = atoi((char *)xmlGetProp(child, (const xmlChar *)"id"));
		vec[i].x = atof((char *)xmlGetProp(child, (const xmlChar *)"x"));
		vec[i].y = atof((char *)xmlGetProp(child, (const xmlChar *)"y"));
		vec[i].z = atof((char *)xmlGetProp(child, (const xmlChar *)"z"));

		i++;
	}

	r = anim->NewKeyFrameFromData(time, count, vert, vec);
	return r;
}

int tMesh::SaveToFile(const char *file)
{
	printf("CMesh::SaveToFile does not do anything at the moment.\n");
	return 0;
	/*xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr cur;
	xmlNodePtr child;
	xmlNodePtr child2;
	xmlChar *buf;
	int buffer_size;
	int i;
	
	vector<CMeshMaterial *>::iterator m;
	vector<CVertex *>::iterator v;
	CVertex *vt;
	vector<CTriangle *>::iterator t;
	map<string, CMeshPosition *>::iterator p;
	map<CVertex *, CVector>::iterator vp;
	CVertex *pv;
	CVector pp;
	vector<CAnimation *>::iterator a;
	CKeyFrame *k;
	vector<CEntity *>::iterator e;
	map<string, CEntityAttribute *>::iterator ea;

	SetIDs();
	ChangePosition(idle_pose);

	doc = xmlNewDoc((const xmlChar *)"1.0");
	root = xmlNewNode(0, (const xmlChar *)"tmesh");
	xmlNewProp(root, (const xmlChar *)"version", (const xmlChar *)TEM_CURRENT_VERSION_STRING);
	xmlDocSetRootElement(doc, root);

	for(m=materials.begin(); m!=materials.end(); m++)
	{
		if((*m)->GetName().length() == 0)
			continue;

		cur = xmlNewNode(0, (const xmlChar *)"material");
		xmlNewProp(cur, (const xmlChar *)"name", (const xmlChar *)(*m)->GetName().c_str());


		CMeshMaterial::Diffuse diffuse = (*m)->GetDiffuse();
		CMeshMaterial::Specular specular = (*m)->GetSpecular();
		CMeshMaterial::Normal normal = (*m)->GetNormal();
		CMeshMaterial::Height height = (*m)->GetHeight();

		if(diffuse.enabled)
		{
			child = xmlNewNode(0, (const xmlChar *)"diffuse");
			xmlNewProp(child, (const xmlChar *)"file", (const xmlChar *)diffuse.filename.c_str());
			xmlAddChild(cur, child);
		}

		if(specular.enabled)
		{
			child = xmlNewNode(0, (const xmlChar *)"specular");
			xmlNewProp(child, (const xmlChar *)"file", (const xmlChar *)specular.filename.c_str());
			xmlNewProp(child, (const xmlChar *)"exponent", (const xmlChar *)ftoa(specular.exponent));
			xmlAddChild(cur, child);
		}

		if(normal.enabled)
		{
			child = xmlNewNode(0, (const xmlChar *)"normal");
			xmlNewProp(child, (const xmlChar *)"file", (const xmlChar *)normal.filename.c_str());
			xmlAddChild(cur, child);
		}

		if(height.enabled)
		{
			child = xmlNewNode(0, (const xmlChar *)"height");
			xmlNewProp(child, (const xmlChar *)"file", (const xmlChar *)height.filename.c_str());
			xmlNewProp(child, (const xmlChar *)"factor", (const xmlChar *)ftoa(height.factor));
			xmlAddChild(cur, child);
		}

		xmlAddChild(root, cur);
	}

	for(v=vertices.begin(); v!=vertices.end(); v++)
	{
		vt = *v;
		cur = xmlNewNode(0, (const xmlChar *)"vertex");
		xmlNewProp(cur, (const xmlChar *)"id", (const xmlChar *)itoa(vt->id));
		xmlNewProp(cur, (const xmlChar *)"x", (const xmlChar *)ftoa(vt->x));
		xmlNewProp(cur, (const xmlChar *)"y", (const xmlChar *)ftoa(vt->y));
		xmlNewProp(cur, (const xmlChar *)"z", (const xmlChar *)ftoa(vt->z));
		xmlNewProp(cur, (const xmlChar *)"u", (const xmlChar *)ftoa(vt->uv.x));
		xmlNewProp(cur, (const xmlChar *)"v", (const xmlChar *)ftoa(vt->uv.y));
		xmlNewProp(cur, (const xmlChar *)"nx", (const xmlChar *)ftoa(vt->normal.x));
		xmlNewProp(cur, (const xmlChar *)"ny", (const xmlChar *)ftoa(vt->normal.y));
		xmlNewProp(cur, (const xmlChar *)"nz", (const xmlChar *)ftoa(vt->normal.z));
		xmlAddChild(root, cur);
	}

    for(t=triangles.begin(); t!=triangles.end(); t++)
	{
		cur = xmlNewNode(0, (const xmlChar *)"triangle");
		xmlNewProp(cur, (const xmlChar *)"mat", (const xmlChar *)(*t)->m_name);
		for(i=0; i<3; i++)
		{
			child = xmlNewNode(0, (const xmlChar *)"vertex");
			xmlNewProp(child, (const xmlChar *)"id", (const xmlChar *)itoa((*t)->v[i]->id));
			//attr = xmlNewProp(child, (const xmlChar *)"u", (const xmlChar *)ftoa((*t)->tex_coord[i].x));
			//attr = xmlNewProp(child, (const xmlChar *)"v", (const xmlChar *)ftoa((*t)->tex_coord[i].y));
			xmlAddChild(cur, child);
		}
		xmlAddChild(root, cur);
	}

	for(p=custom_positions.begin(); p!=custom_positions.end(); p++)
	{
		cur = xmlNewNode(0, (const xmlChar *)"position");
		xmlNewProp(cur, (const xmlChar *)"name", (const xmlChar *)p->first.c_str());
		for(vp=p->second->vertices.begin(); vp!=p->second->vertices.end(); vp++)
		{
			child = xmlNewNode(0, (const xmlChar *)"vertex");
			xmlNewProp(child, (const xmlChar *)"id", (const xmlChar *)itoa(vp->v->id));
			xmlNewProp(child, (const xmlChar *)"x", (const xmlChar *)ftoa(vp->p.x));
			xmlNewProp(child, (const xmlChar *)"y", (const xmlChar *)ftoa(vp->p.y));
			xmlNewProp(child, (const xmlChar *)"z", (const xmlChar *)ftoa(vp->p.z));
			xmlAddChild(cur, child);
		}
		xmlAddChild(root, cur);
	}

	for(a=animations.begin(); a!=animations.end(); a++)
	{
		cur = xmlNewNode(0, (const xmlChar *)"animation");
		xmlNewProp(cur, (const xmlChar *)"name", (const xmlChar *)(*a)->GetName());
		xmlNewProp(cur, (const xmlChar *)"len", (const xmlChar *)ftoa((*a)->GetLength()));
		for(k=(*a)->key_first; k; k=k->chain_next)
		{
			child = xmlNewNode(0, (const xmlChar *)"frame");
			xmlNewProp(child, (const xmlChar *)"time", (const xmlChar *)ftoa(k->time));
			for(vp=k->pos_first; vp; vp=vp->chain_next)
			{
				child2 = xmlNewNode(0, (const xmlChar *)"vertex");
				xmlNewProp(child2, (const xmlChar *)"id", (const xmlChar *)itoa(vp->v->id));
				xmlNewProp(child2, (const xmlChar *)"x", (const xmlChar *)ftoa(vp->p.x));
				xmlNewProp(child2, (const xmlChar *)"y", (const xmlChar *)ftoa(vp->p.y));
				xmlNewProp(child2, (const xmlChar *)"z", (const xmlChar *)ftoa(vp->p.z));
				xmlAddChild(child, child2);
			}
			xmlAddChild(cur, child);
		}
		xmlAddChild(root, cur);
	}

	for(e=entities.begin(); e!=entities.end(); e++)
	{
		cur = xmlNewNode(0, (const xmlChar *)"entity");
		xmlNewProp(cur, (const xmlChar *)"name", (const xmlChar *)(*e)->name.c_str());
		xmlNewProp(cur, (const xmlChar *)"group", (const xmlChar *)(*e)->group.c_str());
		for(ea=(*e)->attributes.begin(); ea!=(*e)->attributes.end(); ea++)
		{
			switch(ea->second->type)
			{
			case CEntityAttribute::VECTOR:
				child = xmlNewNode(0, (const xmlChar *)"vec3");
				xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)ea->first.c_str());
				xmlNewProp(child, (const xmlChar *)"x", (const xmlChar *)ftoa(ea->second->vec_v.x));
				xmlNewProp(child, (const xmlChar *)"y", (const xmlChar *)ftoa(ea->second->vec_v.y));
				xmlNewProp(child, (const xmlChar *)"z", (const xmlChar *)ftoa(ea->second->vec_v.z));
				break;
			case CEntityAttribute::VECTOR2:
				child = xmlNewNode(0, (const xmlChar *)"vec2");
				xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)ea->first.c_str());
				xmlNewProp(child, (const xmlChar *)"x", (const xmlChar *)ftoa(ea->second->vec2_v.x));
				xmlNewProp(child, (const xmlChar *)"y", (const xmlChar *)ftoa(ea->second->vec2_v.y));
				break;
			case CEntityAttribute::FLOAT:
				child = xmlNewNode(0, (const xmlChar *)"float");
				xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)ea->first.c_str());
				xmlNewProp(child, (const xmlChar *)"v", (const xmlChar *)ftoa(ea->second->float_v));
				break;
			case CEntityAttribute::INT:
				child = xmlNewNode(0, (const xmlChar *)"int");
				xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)ea->first.c_str());
				xmlNewProp(child, (const xmlChar *)"v", (const xmlChar *)itoa(ea->second->int_v));
				break;
			case CEntityAttribute::STRING:
				child = xmlNewNode(0, (const xmlChar *)"string");
				xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)ea->first.c_str());
				xmlNewProp(child, (const xmlChar *)"v", (const xmlChar *)ea->second->string_v.c_str());
				break;
			}
			xmlAddChild(cur, child);
		}
		xmlAddChild(root, cur);
	}

	xmlDocDumpFormatMemory(doc, &buf, &buffer_size, 1);
	int r = xmlSaveFormatFile(file, doc, 1);
	xmlFree(buf);
	xmlFreeDoc(doc);
	return r;*/
}


tEntity *tMesh::ParseEntityNode(xmlNodePtr root)
{
	tEntity *e = new tEntity();
	tEntityAttribute *a;
	xmlNodePtr cur;
	string a_type, a_name;

	e->name = string((const char *)xmlGetProp(root, (const xmlChar *)"name"));
	if(e->name.length() == 0)
	{
		delete e;
		return 0;
	}

	e->group = string((const char *)xmlGetProp(root, (const xmlChar *)"group"));

	for(cur=root->children; cur; cur=cur->next)
	{
		a_type = string((const char *)cur->name);

		a = new tEntityAttribute();

		if(a_type.compare("vec3") == 0)
		{
			a->type = tEntityAttribute::VECTOR;
			a->vec_v = Vec(atof((const char *)xmlGetProp(cur, (const xmlChar *)"x")),
					atof((const char *)xmlGetProp(cur, (const xmlChar *)"y")),
					atof((const char *)xmlGetProp(cur, (const xmlChar *)"z")));
		}
		else if(a_type.compare("vec2") == 0)
		{
			a->type = tEntityAttribute::VECTOR2;
			a->vec2_v = Vec(atof((const char *)xmlGetProp(cur, (const xmlChar *)"x")),
					atof((const char *)xmlGetProp(cur, (const xmlChar *)"y")));
		}
		else if(a_type.compare("float") == 0)
		{
			a->type = tEntityAttribute::FLOAT;
			a->float_v = atof((const char *)xmlGetProp(cur, (const xmlChar *)"v"));
		}
		else if(a_type.compare("int") == 0)
		{
			a->type = tEntityAttribute::INT;
			a->int_v = atoi((const char *)xmlGetProp(cur, (const xmlChar *)"v"));
		}
		else if(a_type.compare("string") == 0)
		{
			a->type = tEntityAttribute::STRING;
			a->string_v = string((const char *)xmlGetProp(cur, (const xmlChar *)"v"));
		}
		else
		{
			delete a;
			continue;
		}

		a_name = string((const char *)xmlGetProp(cur, (const xmlChar *)"name"));

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


tMeshPose *tMesh::GetCustomPoseByName(string name)
{
	try
	{
		return custom_pose.at(name);
	}
	catch(...) {}

	return 0;
}

tMeshPose *tMesh::CreateCustomPose(string name)
{
	if(GetCustomPoseByName(name))
		return 0;

	tMeshPose *p = new tMeshPose(this);
	p->CopyFromVertices();
	custom_pose.insert(pair<string, tMeshPose *>(name, p));

	return p;
}

void tMesh::ChangePose(string name, string idle)
{
	tMeshPose *p;

	if(name == idle)
		current_pose = idle_pose;
	else if((p = GetCustomPoseByName(name)) != 0)
		current_pose = p;
	else
		return;

	animation_mode = false;

	//current_pose->ApplyPose();
}

void tMesh::ChangePose(tMeshPose *pos)
{
	if(pos == current_pose)
		return;

	current_pose = pos;

	animation_mode = false;

	//if(current_pose)
	//	current_pose->ApplyPose();
}

tMeshPose *tMesh::GetCurrentPose(void)
{
	return current_pose;
}

void tMesh::CopyPoseFromVertices(void)
{
	if(current_pose)
		current_pose->CopyFromVertices();
}


string tMesh::GetCurrentPoseName(string idle) // deprecated
{
	string r;

	if(!current_pose)
		return 0;

	if(current_pose == idle_pose)
		return idle;
	else
	{
		map<string, tMeshPose *>::iterator i;
		for(i=custom_pose.begin(); i!=custom_pose.end(); i++)
		{
			if(i->second == current_pose)
				return i->first;
		}
	}

	return string();
}



tAnimation *tMesh::CreateAnimation(const char *name, float len)
{
	if(GetAnimationByName(name))
		return 0;

	return new tAnimation(this, name, len);
}

void tMesh::ChangeAnimation(tAnimation *a)
{
	if(a->mesh != this)
		return;

	ResetAnimationFinished();
	current_animation = a;
	current_animation->SetTime(0.0);
	animation_mode = true;
	//RefreshAllVBOs();
}

void tMesh::ChangeAnimation(const char *name)
{
	tAnimation *a;

	a = GetAnimationByName(name);

	if(!a)
		return;

	ChangeAnimation(a);
}

tAnimation *tMesh::GetAnimationByName(const char *name)
{
	vector<tAnimation *>::iterator a;

	for(a=animations.begin(); a!=animations.end(); a++)
		if(strcmp(name, (*a)->GetName()) == 0)
			return *a;
	return 0;

}

tAnimation *tMesh::GetCurrentAnimation(void)
{
	return current_animation;
}

char *tMesh::GetCurrentAnimationName(void)
{
	char *r;
	char *n;

	if(!current_animation)
		return 0;

	n = current_animation->GetName();

	r = new char[strlen(n) + 1];
	strcpy(r, n);
	return r;
}

void tMesh::PlayAnimation(float t)
{
	if(!current_animation)
		return;
	anim_finished = current_animation->Play(t, loop_anim);
	current_animation->ApplyCurrentFrame();
}

void tMesh::ApplyAnimation(void)
{
	if(current_animation)
		current_animation->ApplyCurrentFrame();
}

bool CompareTriangleDist(tTriangle *a, tTriangle *b)
{
	return a->cam_dist > b->cam_dist;
}

bool CompareTriangleMaterial(tTriangle *a, tTriangle *b)
{
	//if(a->mat == b->mat)
	//	return CompareDist(a, b);
	//else
	if(b->mat->GetTransparent() == a->mat->GetTransparent())
		return a->mat < b->mat;
	else
		return b->mat->GetTransparent() && !a->mat->GetTransparent();
}

bool CompareMaterialTransparency(tMeshMaterial *a, tMeshMaterial *b)
{
	if(b->GetTransparent() == a->GetTransparent())
		return a < b;
	else
		return b->GetTransparent() && !a->GetTransparent();
}

void tMesh::SortTriangles(tVector cam)
{
	//vector<CTriangle *>::iterator t;
	//vector<CVertex *>::iterator v;

	/*for(v=vertices.begin(); v!=vertices.end(); v++)
		(*v)->cam_dist = (cam - ApplyMatrix4(transformation, **v)).SquaredLen();
	
	for(t=triangles.begin(); t!=triangles.end(); t++)
		(*t)->cam_dist = ((*t)->v[0]->cam_dist + (*t)->v[1]->cam_dist + (*t)->v[2]->cam_dist) * (1.0/3.0);*/

	sort(triangles.begin(), triangles.end(), CompareTriangleMaterial);
	refresh_vbos = true;
}

void tMesh::SortMaterials(void)
{
	sort(materials.begin(), materials.end(), CompareMaterialTransparency);
}


