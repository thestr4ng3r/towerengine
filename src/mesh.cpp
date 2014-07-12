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


CVector light_dir;
CVector light_pos;
CVector cam_pos;
CVector ambient_color;
CVector light_attenuation;


const float CMesh::identity_matrix[16] = { 1.0, 0.0, 0.0, 0.0,
										  0.0, 1.0, 0.0, 0.0,
										  0.0, 0.0, 1.0, 0.0,
										  0.0, 0.0, 0.0, 1.0 };




void CMesh::CalculateNormalsSolid(void)
{
	vector<CTriangle *>::iterator i;
	for(i=triangles.begin(); i!=triangles.end(); i++)
		(*i)->CalculateNormalSolid();
}


CMesh::CMesh(void)
{
	idle_pose = 0;
	vao = 0;
	//mat_indices = 0;
	refresh_func = 0;
	refresh_vbos = true;
	refresh_ibos = true;
	outdated_vertices.clear();
	vertex_indices.clear();
	entities.clear();
	Create();
}

CMesh::~CMesh(void)
{
	Delete();
}

float CMesh::transformation[16] = 
		{ 1.0, 0.0, 0.0, 0.0,
		  0.0, 1.0, 0.0, 0.0,
		  0.0, 0.0, 1.0, 0.0,
		  0.0, 0.0, 0.0, 1.0 };

float CMesh::color[4] = { 1.0, 1.0, 1.0, 1.0 };

void CMesh::LoadIdentity(void)
{
	memcpy(transformation, identity_matrix, 16 * sizeof(float));
	Color(Vec(1.0, 1.0, 1.0), 1.0);
}

void CMesh::Color(CVector c, float a)
{
	color[0] = c.x;
	color[1] = c.y;
	color[2] = c.z;
	color[3] = a;
}

void CMesh::Translate(CVector v)
{
	float m[16] =
		{ 1.0, 0.0, 0.0, v.x,
		  0.0, 1.0, 0.0, v.y,
		  0.0, 0.0, 1.0, v.z,
		  0.0, 0.0, 0.0, 1.0 };

	CombineMatrix4(transformation, m, transformation);
}

void CMesh::RotateX(float a)
{
	float m[16] = 
		{ (float)cos(a), -(float)sin(a), 0.0, 0.0,
		  (float)sin(a), (float)cos(a),  0.0, 0.0,
		  0.0,    0.0,     1.0, 0.0,
		  0.0,    0.0,     0.0, 1.0 };
	CombineMatrix4(transformation, m, transformation);
}

void CMesh::RotateY(float a)
{
	float m[16] = 
		{ (float)cos(a), 0.0,  (float)sin(a), 0.0,
		  0.0,    1.0,     0.0, 0.0,
		  -(float)sin(a),0.0,  (float)cos(a), 0.0,
		  0.0,    0.0,     0.0, 1.0 };
	CombineMatrix4(transformation, m, transformation);
}

void CMesh::RotateZ(float a)
{
	float m[16] = 
		{ 1.0,    0.0,     0.0, 0.0,
		  0.0, (float)cos(a), -(float)sin(a), 0.0,
		  0.0, (float)sin(a),  (float)cos(a), 0.0,
		  0.0,    0.0,     0.0, 1.0 };
	CombineMatrix4(transformation, m, transformation);
}

void CMesh::Rotate(CVector v)
{
	RotateX(v.x);
	RotateY(v.y);
	RotateZ(v.z);
}

void CMesh::Scale(CVector v)
{
	transformation[0] *= v.x;
	transformation[1] *= v.x;
	transformation[3] *= v.x;
	transformation[5] *= v.y;
	transformation[6] *= v.y;
	transformation[7] *= v.y;
	transformation[9] *= v.z;
	transformation[10] *= v.z;
	transformation[11] *= v.z;
}

void CMesh::ApplyTransformation(void)
{
	ApplyMatrix(transformation);
}

void CMesh::ApplyMatrix(float m[16])
{
	vector<CVertex *>::iterator v;

	for(v=vertices.begin(); v!=vertices.end(); v++)
		(*v)->SetVector(ApplyMatrix4(m, **v));
}

void CMesh::SetXY(CVector x, CVector y)
{
	CVector z = Cross(x, y);
	SetXYZ(x, y, z);
}

void CMesh::SetYZ(CVector y, CVector z)
{
	CVector x = Cross(y, z);
	SetXYZ(x, y, z);
}

void CMesh::SetXZ(CVector x, CVector z)
{
	CVector y = Cross(x, z);
	SetXYZ(x, y, z);
}

void CMesh::SetXYZ(CVector x, CVector y, CVector z)
{
	float m[16] =
			{ x.x, y.x, z.x, 0.0,
			  x.y, y.y, z.y, 0.0,
			  x.z, y.z, z.z, 0.0,
			  0.0, 0.0, 0.0, 1.0 };

	CombineMatrix4(transformation, m, transformation);
}


void CMesh::Create(void)
{
	Delete();

	vao = new VAO();
	current_pose = idle_pose = new CMeshPose(this);
	//vertex_vbo = new VBO<float>(3, vao);
	normal_vbo = new VBO<float>(3, vao);
	tang_vbo = new VBO<float>(3, vao);
	bitang_vbo = new VBO<float>(3, vao);
	face_normal_vbo = new VBO<float>(3, vao);
	uvcoord_vbo = new VBO<float>(2, vao);
	outdated_vertices.clear();
	data_count = 0;
	refresh_func = 0;
	refresh_vbos = true;
	refresh_ibos = true;
	idle_material = new CMaterial(this, string());
	//VAO::UnBind();

	SetWireframe(0);
	SetAnimationLoop(1);
	ResetAnimationFinished();
}

void CMesh::Delete(void)
{
	if(!GetState())
		return;

	map<string, CMeshPose *>::iterator cpi;
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

	data_count = 0;
}

void CMesh::DeleteVBOData(void)
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

VBO<float> *CMesh::CreateFloatVBO(int components)
{
	return new VBO<float>(components, vao);
}


CVertex *CMesh::CreateVertex(CVector v)
{
	CVertex *o;
	o = new CVertex(v, this);
	refresh_vbos = false;
	return o;
}

CTriangle *CMesh::CreateTriangle(CVertex *v1, CVertex *v2, CVertex *v3, CVector color, char material[100], CVector t1, CVector t2, CVector t3) // TODO: Optimieren!
{
    if(!GetState())
        return 0;

	refresh_ibos = true;

    return CTriangle::CreateTriangle(v1, v2, v3, color, material, t1, t2, t3, this);
}

CTriangle *CMesh::CreateTriangleAuto(CVector v1, CVector v2, CVector v3, CVector color, char material[100], CVector t1, CVector t2, CVector t3)
{
	CVertex *vert[3];
	vector<CVertex *>::iterator verti;
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

CEntity *CMesh::CreateEntity(string name, string group)
{
	CEntity *e = new CEntity();
	e->name = name;
	e->group = group;
	entities.push_back(e);
	return e;
}

CMaterial *CMesh::GetMaterialByName(string name)
{
    vector<CMaterial *>::iterator m;

    for(m=materials.begin(); m!=materials.end(); m++)
    {
        if((*m)->GetName().compare(name) == 0)
            return *m;
    }

    return 0;
}

CVertex *CMesh::GetVertexByID(int id)
{
	map<int, CVertex *>::iterator i = vertex_indices.find(id);
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

void CMesh::SetVertexId(CVertex *v, int id)
{
	if(v->id != 0)
		vertex_indices.erase(v->id);

	v->id = id;
	vertex_indices.insert(pair<int, CVertex *>(id, v));
}

void CMesh::AddVertex(CVertex *v)
{
	vector<CVertex *>::iterator i;

	for(i=vertices.begin(); i!=vertices.end(); i++)
		if(*i == v)
			return;
	vertices.push_back(v);

	refresh_vbos = true;
}

void CMesh::AddTriangle(CTriangle *t)
{
	vector<CTriangle *>::iterator i;

	for(i=triangles.begin(); i!=triangles.end(); i++)
		if(*i == t)
			return;
	triangles.push_back(t);
	refresh_vbos = true;
	refresh_ibos = true;
}

void CMesh::AddMaterial(CMaterial *m)
{
	vector<CMaterial *>::iterator i;

	for(i=materials.begin(); i!=materials.end(); i++)
		if(*i == m)
			return;
	materials.push_back(m);
	refresh_ibos = true;
}

void CMesh::AddCustomPose(string name, CMeshPose *p)
{
	if(custom_pose.find(name) != custom_pose.end())
		return;

	custom_pose.insert(pair<string, CMeshPose *>(name, p));
	//refresh_vbo = true;
}

void CMesh::AddAnimation(CAnimation *a)
{
	vector<CAnimation *>::iterator i;

	for(i=animations.begin(); i!=animations.end(); i++)
		if(*i == a)
			return;
	animations.push_back(a);
	//refresh_vbo = true;
}

void CMesh::RemoveVertex(CVertex *v)
{
	vector<CVertex *>::iterator i;

	for(i=vertices.begin(); i!=vertices.end(); i++)
		if(*i == v)
		{
			vertices.erase(i);
			break;
		}
	refresh_vbos = true;
	refresh_ibos = true;
}

void CMesh::RemoveTriangle(CTriangle *t)
{
	vector<CTriangle *>::iterator i;

	for(i=triangles.begin(); i!=triangles.end(); i++)
		if(*i == t)
		{
			triangles.erase(i);
			break;
		}
	refresh_ibos = true;
	//refresh_vbo = true;
}

void CMesh::RemoveMaterial(CMaterial *m)
{
	vector<CMaterial *>::iterator i;

	for(i=materials.begin(); i!=materials.end(); i++)
		if(*i == m)
		{
			materials.erase(i);
			break;
		}
	refresh_ibos = true;
	//refresh_vbo = true;
}

void CMesh::RemoveCustomPose(string name)
{
	map<string, CMeshPose *>::iterator i;

	i = custom_pose.find(name);

	if(i != custom_pose.end())
		custom_pose.erase(i);
}

void CMesh::RemoveAnimation(CAnimation *a)
{
	vector<CAnimation *>::iterator i;

	for(i=animations.begin(); i!=animations.end(); i++)
		if(*i == a)
		{
			animations.erase(i);
			break;
		}
	//refresh_vbo = true;
}

void CMesh::RemoveEntity(CEntity *e)
{
	vector<CEntity *>::iterator i;

	for(i=entities.begin(); i!=entities.end(); i++)
		if(*i == e)
		{
			entities.erase(i);
			break;
		}
}

void CMesh::SetIDs(void)
{
	int i;
	vector<CVertex *>::iterator v;
	vector<CAnimation *>::iterator a;
	CKeyFrame *f;
	map<string, CMeshPose *>::iterator p;

	for(v=vertices.begin(), i=0; v!=vertices.end(); v++, i++)
		SetVertexId((*v), i);

	for(a=animations.begin(); a!=animations.end(); a++)
		for(f=(*a)->key_first; f; f=f->chain_next, i++)
			f->id = i;

	for(p=custom_pose.begin(), i=0; p!=custom_pose.end(); p++)
		p->second->id = i;
}

void CMesh::SetTriangleMaterials(void)
{
    vector<CTriangle *>::iterator t;

    for(t=triangles.begin(); t!=triangles.end(); t++)
    {
        (*t)->mat = GetMaterialByName((*t)->m_name);
        if(!(*t)->mat)
        	(*t)->mat = GetIdleMaterial();
    }
	//refresh_vbo = true;
}

void CMesh::AssignVertexArrayPositions(void)
{
	vector<CVertex *>::iterator i;
	int c;

	for(c=0, i=vertices.begin(); i!=vertices.end(); c++, i++)
		(*i)->index = c;

	refresh_vbos = true;
}

void CMesh::RefreshAllVBOs(void)
{
	vector<CTriangle *>::iterator i;
	vector<CMaterial *>::iterator m;
	vector<CVertex *>::iterator v;
	CVertex *vt;
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

void CMesh::RefreshIBOs(void)
{
	vector<CTriangle *>::iterator i;
	vector<CMaterial *>::iterator m;
	CTriangle *t;
	CMaterial *mt;
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

void CMesh::PutToGL(CVector cam, int both_sides)
{
	vector<CMaterial *>::iterator i;
	VBO<float> *vertex_vbo, *vertex2_vbo;
	CKeyFrame *a, *b;
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

	vertex_vbo->SetAttribute(CFaceShader::vertex_attribute, GL_FLOAT);
	if(vertex2_vbo)
	{
		CEngine::GetCurrentFaceShader()->SetVertexMix(mix);
		vertex2_vbo->SetAttribute(CFaceShader::vertex2_attribute, GL_FLOAT);
	}
	else
		CEngine::GetCurrentFaceShader()->SetVertexMix(0.0);

	normal_vbo->SetAttribute(CFaceShader::normal_attribute, GL_FLOAT);
	tang_vbo->SetAttribute(CFaceShader::tang_attribute, GL_FLOAT);
	bitang_vbo->SetAttribute(CFaceShader::bitang_attribute, GL_FLOAT);
	//face_normal_vbo->SetAttribute(CEngine::GetFaceShader()->face_normal_attribute, GL_FLOAT);
	uvcoord_vbo->SetAttribute(CFaceShader::uvcoord_attribute, GL_FLOAT);

	if(refresh_ibos)
		RefreshIBOs();

	//CEngine::GetFaceShader()->BindShader();
	CEngine::GetCurrentFaceShader()->SetTwoSide(both_sides);
	CEngine::GetCurrentFaceShader()->SetTransformation(CMesh::transformation);
	CEngine::GetCurrentFaceShader()->SetDiffuseColor2(Vec(color[0], color[1], color[2]), color[3]);

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

int CMesh::GetState(void)
{
    return 1;
}

bool CMesh::LoadFromFile(const char *file, int no_material)
{
	bool r;
	char *path;
	struct stat s;
	
	if(stat(file, &s) != 0 && errno == ENOENT)
	    return 0;
	
	path = PathOfFile(file);

	r = LoadFromFile_xml(file, path, no_material);

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

bool CMesh::LoadFromFile_xml(const char *file, const char *path, int no_material)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	char *version_string;

	doc = xmlParseFile(file);

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
		printf("WARNING: File \"%s\" does not contain version info. Assuming it is %s.\n", file, TEM_VERSION_0_1_STRING);
		file_version = TEM_VERSION_0_1;
	}
	else
	{
		file_version = TEMVersionFromString(version_string);
		if(file_version == -1)
		{
			printf("WARNING: Version \"%s\" of file \"%s\" could not be recognized. Assuming it is %s.\n",
					version_string, file, TEM_CURRENT_VERSION_STRING);
			file_version = TEM_CURRENT_VERSION;
		}
		else if(file_version != TEM_CURRENT_VERSION)
		{
			printf("WARNING: Version \"%s\" of file \"%s\" is outdated. Use \"%s\" instead.\n",
					version_string, file, TEM_CURRENT_VERSION_STRING);
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

	refresh_vbos = true;
	refresh_ibos = true;

	return 1;
}

CVertex *CMesh::ParseVertexNode(xmlNodePtr cur)
{
	int id;
	CVector p;
	CVector normal;
	CVector2 uv;
	CVertex *r;

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

	r = new CVertex(p, this);
	SetVertexId(r, id);
	r->uv_set = file_version >= TEM_VERSION_0_2;
	r->uv = uv;
	r->normal = normal;
	return r;
}

CMaterial *CMesh::ParseMaterialNode(xmlNodePtr cur, const char *path)
{
	xmlNodePtr child;
	bool diffuse_enabled, specular_enabled, normal_enabled, height_enabled;
	string diffuse_file; float ambient; CVector diffuse_color;
	string specular_file; float exponent; CVector specular_color;
	string normal_file;
	string height_file; float height_factor;
	string name;
	xmlChar *temp;
	CMaterial *r;

	diffuse_enabled = specular_enabled = normal_enabled = height_enabled = false;
	ambient = 1.0;
	diffuse_color = Vec(1.0, 1.0, 1.0);
	specular_color = Vec(0.0, 0.0, 0.0);

	name = string((const char *)xmlGetProp(cur, (const xmlChar *)"name"));

	child = cur->children;
	while(child)
	{
		if(!xmlStrcmp(child->name, (const xmlChar *)"diffuse"))
		{
			if((temp = xmlGetProp(child, (const xmlChar *)"file")))
				diffuse_file = string((const char *)temp);
			diffuse_enabled = diffuse_file.length() > 0;
			if((temp = xmlGetProp(child, (const xmlChar *)"ambient")))
				ambient = atof((const char *)temp);
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
				specular_file = string((const char *)temp);
			specular_enabled = specular_file.length() > 0;
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
				normal_file = string((const char *)temp);
			normal_enabled = normal_file.length() > 0;
		}
		if(!xmlStrcmp(child->name, (const xmlChar *)"height"))
		{
			if((temp = xmlGetProp(child, (const xmlChar *)"file")))
				height_file = string((const char *)temp);
			height_enabled = height_file.length() > 0;
			if((temp = xmlGetProp(child, (const xmlChar *)"factor")))
				height_factor = atof((const char *)temp);
			else
				height_factor = 1.0;
		}
		child = child->next;
	}

	r = new CMaterial(this, name);

	if(diffuse_file.compare("$NONE") == 0)
		diffuse_enabled = false;
	if(specular_file.compare("$NONE") == 0)
		specular_enabled = false;
	if(normal_file.compare("$NONE") == 0)
		normal_enabled = false;
	if(height_file.compare("$NONE") == 0)
		height_enabled = false;

	r->SetDiffuse(diffuse_enabled, diffuse_file, diffuse_color, ambient);
	r->SetSpecular(specular_enabled, specular_file, specular_color, exponent);
	r->SetNormal(normal_enabled, normal_file);
	r->SetHeight(height_enabled, height_file, height_factor);
	r->Load(string(path));

	return r;
}

CTriangle *CMesh::ParseTriangleNode(xmlNodePtr cur)
{
	char *material;
	int v[3];
	int i;
	CVector uv[3];
	CVertex *vt[3];
	CTriangle *r;
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

CMeshPose *CMesh::ParsePoseNode(xmlNodePtr cur)
{
	char *name;
	xmlNodePtr child;
	int count;
	int i;
	int *vert;
	CVector *vec;
	CMeshPose *r;

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
	vec = new CVector[count];

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

	r = new CMeshPose(this);
	r->CopyFromData(count, vert, vec);
	AddCustomPose(name, r);
	return r;
}

CAnimation *CMesh::ParseAnimationNode(xmlNodePtr cur)
{
	char *name;
	float len;
	xmlNodePtr child;
	CAnimation *r;

	name = (char *)xmlGetProp(cur, (const xmlChar *)"name");
	len = atof((char *)xmlGetProp(cur, (const xmlChar *)"len"));

	r = new CAnimation(this, name, len, 0);

	for(child = cur->children; child; child = child->next)
	{
		if(xmlStrcmp(child->name, (const xmlChar *)"frame"))
			continue;
		ParseKeyFrameNode(child, r);
	}

	return r;
}

CKeyFrame *CMesh::ParseKeyFrameNode(xmlNodePtr cur, CAnimation *anim)
{
	float time;
	xmlNodePtr child;
	int count;
	int i;
	int *vert;
	CVector *vec;
	CKeyFrame *r;

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
	vec = new CVector[count];


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

int CMesh::SaveToFile(const char *file)
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
	
	vector<CMaterial *>::iterator m;
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


		CMaterial::Diffuse diffuse = (*m)->GetDiffuse();
		CMaterial::Specular specular = (*m)->GetSpecular();
		CMaterial::Normal normal = (*m)->GetNormal();
		CMaterial::Height height = (*m)->GetHeight();

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


CEntity *CMesh::ParseEntityNode(xmlNodePtr root)
{
	CEntity *e = new CEntity();
	CEntityAttribute *a;
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

		a = new CEntityAttribute();

		if(a_type.compare("vec3") == 0)
		{
			a->type = CEntityAttribute::VECTOR;
			a->vec_v = Vec(atof((const char *)xmlGetProp(cur, (const xmlChar *)"x")),
					atof((const char *)xmlGetProp(cur, (const xmlChar *)"y")),
					atof((const char *)xmlGetProp(cur, (const xmlChar *)"z")));
		}
		else if(a_type.compare("vec2") == 0)
		{
			a->type = CEntityAttribute::VECTOR2;
			a->vec2_v = Vec(atof((const char *)xmlGetProp(cur, (const xmlChar *)"x")),
					atof((const char *)xmlGetProp(cur, (const xmlChar *)"y")));
		}
		else if(a_type.compare("float") == 0)
		{
			a->type = CEntityAttribute::FLOAT;
			a->float_v = atof((const char *)xmlGetProp(cur, (const xmlChar *)"v"));
		}
		else if(a_type.compare("int") == 0)
		{
			a->type = CEntityAttribute::INT;
			a->int_v = atoi((const char *)xmlGetProp(cur, (const xmlChar *)"v"));
		}
		else if(a_type.compare("string") == 0)
		{
			a->type = CEntityAttribute::STRING;
			a->string_v = string((const char *)xmlGetProp(cur, (const xmlChar *)"v"));
		}
		else
		{
			delete a;
			continue;
		}

		a_name = string((const char *)xmlGetProp(cur, (const xmlChar *)"name"));

		e->attributes.insert(pair<string, CEntityAttribute *>(a_name, a));
	}

	entities.push_back(e);

	return e;
}



map<string, CEntity *> CMesh::GetEntitiesInGroup(const char *group)
{
	map<string, CEntity *> r;
	vector<CEntity *>::iterator i;

	for(i=entities.begin(); i!=entities.end(); i++)
		if((*i)->group.compare(group) == 0)
			r.insert(pair<string, CEntity *>((*i)->name, *i));

	return r;
}


CMeshPose *CMesh::GetCustomPoseByName(string name)
{
	try
	{
		return custom_pose.at(name);
	}
	catch(...) {}

	return 0;
}

CMeshPose *CMesh::CreateCustomPose(string name)
{
	if(GetCustomPoseByName(name))
		return 0;

	CMeshPose *p = new CMeshPose(this);
	p->CopyFromVertices();
	custom_pose.insert(pair<string, CMeshPose *>(name, p));

	return p;
}

void CMesh::ChangePose(string name, string idle)
{
	CMeshPose *p;

	if(name == idle)
		current_pose = idle_pose;
	else if((p = GetCustomPoseByName(name)) != 0)
		current_pose = p;
	else
		return;

	animation_mode = false;

	//current_pose->ApplyPose();
}

void CMesh::ChangePose(CMeshPose *pos)
{
	if(pos == current_pose)
		return;

	current_pose = pos;

	animation_mode = false;

	//if(current_pose)
	//	current_pose->ApplyPose();
}

CMeshPose *CMesh::GetCurrentPose(void)
{
	return current_pose;
}

void CMesh::CopyPoseFromVertices(void)
{
	if(current_pose)
		current_pose->CopyFromVertices();
}


string CMesh::GetCurrentPoseName(string idle) // deprecated
{
	string r;

	if(!current_pose)
		return 0;

	if(current_pose == idle_pose)
		return idle;
	else
	{
		map<string, CMeshPose *>::iterator i;
		for(i=custom_pose.begin(); i!=custom_pose.end(); i++)
		{
			if(i->second == current_pose)
				return i->first;
		}
	}

	return string();
}



CAnimation *CMesh::CreateAnimation(const char *name, float len)
{
	if(GetAnimationByName(name))
		return 0;

	return new CAnimation(this, name, len);
}

void CMesh::ChangeAnimation(CAnimation *a)
{
	if(a->mesh != this)
		return;

	ResetAnimationFinished();
	current_animation = a;
	current_animation->SetTime(0.0);
	animation_mode = true;
	//RefreshAllVBOs();
}

void CMesh::ChangeAnimation(const char *name)
{
	CAnimation *a;

	a = GetAnimationByName(name);

	if(!a)
		return;

	ChangeAnimation(a);
}

CAnimation *CMesh::GetAnimationByName(const char *name)
{
	vector<CAnimation *>::iterator a;

	for(a=animations.begin(); a!=animations.end(); a++)
		if(strcmp(name, (*a)->GetName()) == 0)
			return *a;
	return 0;

}

CAnimation *CMesh::GetCurrentAnimation(void)
{
	return current_animation;
}

char *CMesh::GetCurrentAnimationName(void)
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

void CMesh::PlayAnimation(float t)
{
	if(!current_animation)
		return;
	anim_finished = current_animation->Play(t, loop_anim);
	current_animation->ApplyCurrentFrame();
}

void CMesh::ApplyAnimation(void)
{
	if(current_animation)
		current_animation->ApplyCurrentFrame();
}

bool CompareTriangleDist(CTriangle *a, CTriangle *b)
{
	return a->cam_dist > b->cam_dist;
}

bool CompareTriangleMaterial(CTriangle *a, CTriangle *b)
{
	//if(a->mat == b->mat)
	//	return CompareDist(a, b);
	//else
	if(b->mat->GetTransparent() == a->mat->GetTransparent())
		return a->mat < b->mat;
	else
		return b->mat->GetTransparent() && !a->mat->GetTransparent();
}

bool CompareMaterialTransparency(CMaterial *a, CMaterial *b)
{
	if(b->GetTransparent() == a->GetTransparent())
		return a < b;
	else
		return b->GetTransparent() && !a->GetTransparent();
}

void CMesh::SortTriangles(CVector cam)
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

void CMesh::SortMaterials(void)
{
	sort(materials.begin(), materials.end(), CompareMaterialTransparency);
}


