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

CMaterial *CMaterial::current = 0;

CVector light_dir;
CVector light_pos;
CVector cam_pos;
CVector ambient_color;
CVector light_attenuation;




void CMesh::CalculateNormalsSolid(void)
{
	vector<CTriangle *>::iterator i;
	for(i=triangles.begin(); i!=triangles.end(); i++)
		(*i)->CalculateNormalSolid();
}


CMesh::CMesh(void)
{
	idle_position = 0;
	vao = 0;
	mat_indices = 0;
	refresh_func = 0;
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
	static const float m[16] =
		{ 1.0, 0.0, 0.0, 0.0,
		  0.0, 1.0, 0.0, 0.0,
		  0.0, 0.0, 1.0, 0.0,
		  0.0, 0.0, 0.0, 1.0 };
	memcpy(transformation, m, 16 * sizeof(float));
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
		{ cos(a), -sin(a), 0.0, 0.0,
		  sin(a), cos(a),  0.0, 0.0,
		  0.0,    0.0,     1.0, 0.0,
		  0.0,    0.0,     0.0, 1.0 };
	CombineMatrix4(transformation, m, transformation);
}

void CMesh::RotateY(float a)
{
	float m[16] = 
		{ cos(a), 0.0,  sin(a), 0.0,
		  0.0,    1.0,     0.0, 0.0,
		  -sin(a),0.0,  cos(a), 0.0,
		  0.0,    0.0,     0.0, 1.0 };
	CombineMatrix4(transformation, m, transformation);
}

void CMesh::RotateZ(float a)
{
	float m[16] = 
		{ 1.0,    0.0,     0.0, 0.0,
		  0.0, cos(a), -sin(a), 0.0,
		  0.0, sin(a),  cos(a), 0.0,
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

	current_position = idle_position = new CIdlePosition(this);

	vao = new VAO();
	vertex_vbo = new VBO<float>(3, vao);
	normal_vbo = new VBO<float>(3, vao);
	face_normal_vbo = new VBO<float>(3, vao);
	uvcoord_vbo = new VBO<float>(2, vao);
	outdated_vertices.clear();
	data_count = 0;
	mat_data_count = 0;
	mat_indices = 0;
	mat_list = 0;
	refresh_func = 0;
	idle_material = CreateMaterial("$NONE", "$NONE", 32.0, "$NONE", "$NONE", 0.0, "$NONE", 0.0, cstr("$NONE"));
	//VAO::UnBind();

	SetWireframe(0);
	SetAnimationLoop(1);
	ResetAnimationFinished();
}

void CMesh::Delete(void)
{
	if(!GetState())
		return;

	while(!custom_positions.empty())
		delete custom_positions.back();

	while(!animations.empty())
		delete animations.back();

	if(idle_position)
		delete idle_position;
	
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

	data_count = 0;
}

void CMesh::DeleteVBOData(void)
{
	if(vao)
	{
		delete vao;
		vao = 0;
		delete vertex_vbo;
		delete normal_vbo;
		delete face_normal_vbo;
		delete uvcoord_vbo;
	}

	if(mat_indices)
	{
		delete [] mat_indices;
		delete [] mat_list;
		mat_indices = 0;
		mat_data_count = 0;
	}
}


CMesh *CMesh::Copy(void)
{
	CMesh *r;
	int i;

	r = new CMesh();

	SetIDs();

	vector<CVertex *>::iterator v;
	CVertex *vn;
	for(v=vertices.begin(); v!=vertices.end(); v++)
	{
		vn = new CVertex(r);
		vn->SetVector(**v);
		SetVertexId(vn, (*v)->id);
	}

	vector<CMaterial *>::iterator m;
	CMaterial *mn;
	for(m=materials.begin(); m!=materials.end(); m++)
	{
		mn = new CMaterial(r);
		mn->SetValues((*m)->diffuse_filename, (*m)->specular_filename, (*m)->specular_exponent, (*m)->normal_filename, (*m)->aeb_filename,
				(*m)->bump_factor, (*m)->height_filename, (*m)->height_factor);
		(*m)->tex->Copy(mn->tex);
		mn->name = cstr((*m)->name);
	}

	vector<CTriangle *>::iterator t;
	CTriangle *tn;
	for(t=triangles.begin(); t!=triangles.end(); t++)
	{
		if(strcmp((*t)->m_name, "$NONE") == 0)
			continue;
		tn = new CTriangle(r);
		for(i=0; i<3; i++)
		{
			tn->v[i] = r->GetVertexByID((*t)->v[i]->id);
			tn->tex_coord[i] = (*t)->tex_coord[i];
		}
		tn->SetMaterial((*t)->tex_coord[0], (*t)->tex_coord[1], (*t)->tex_coord[2], (*t)->m_name);
		tn->color = (*t)->color;
		tn->normal[0] = (*t)->normal[0];
		tn->normal[1] = (*t)->normal[1];
		tn->normal[2] = (*t)->normal[2];
	}

	r->SetTriangleMaterials();

	idle_position->Copy(r);

	vector<CCustomPosition *>::iterator cp;
	for(cp=custom_positions.begin(); cp!=custom_positions.end(); cp++)
		(*cp)->Copy(r);

	vector<CAnimation *>::iterator a;
	for(a=animations.begin(); a!=animations.end(); a++)
		(*a)->Copy(r);

	return r;
}




CVertex *CMesh::CreateVertex(CVector v)
{
	CVertex *o;
	o = new CVertex(v, this);
	return o;
}

CMaterial *CMesh::CreateMaterial(const char *diffuse, const char *specular, float exponent, const char *normal, const char *aeb,
			float b_factor, const char *height, float h_factor, char name[100])
{
    if(!GetState())
        return 0;

    return CMaterial::CreateMaterial(diffuse, specular, exponent, normal, aeb, b_factor, height, h_factor, name, this);
}

CMaterial *CMesh::CreateMaterialRelative(const char *path, const char *diffuse, const char *specular, float exponent, const char *normal,
			const char *aeb, float b_factor, const char *height, float h_factor, char name[100])
{
    if(!GetState())
        return 0;

    return CMaterial::CreateMaterialRelative(path, diffuse, specular, exponent, normal, aeb, b_factor, height, h_factor, name, this);
}

CTriangle *CMesh::CreateTriangle(CVertex *v1, CVertex *v2, CVertex *v3, CVector color, char material[100], CVector t1, CVector t2, CVector t3) // TODO: Optimieren!
{
    if(!GetState())
        return 0;
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
	return CreateTriangle(vert[0], vert[1], vert[2],
               color,			     // color
               material,                    // material
               t1,			    // t1
               t2,			    // t2
               t3);			    // t3
}


CMaterial *CMesh::GetMaterialByName(char name[100])
{
    vector<CMaterial *>::iterator m;

    if(strcmp(name, "$NONE") == 0)
        return 0;

    for(m=materials.begin(); m!=materials.end(); m++)
    {
        if(strcmp((*m)->name, name) == 0)
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
}

void CMesh::AddTriangle(CTriangle *t)
{
	vector<CTriangle *>::iterator i;

	for(i=triangles.begin(); i!=triangles.end(); i++)
		if(*i == t)
			return;
	triangles.push_back(t);
}

void CMesh::AddMaterial(CMaterial *m)
{
	vector<CMaterial *>::iterator i;

	for(i=materials.begin(); i!=materials.end(); i++)
		if(*i == m)
			return;
	materials.push_back(m);
}

void CMesh::AddCustomPosition(CCustomPosition *p)
{
	vector<CCustomPosition *>::iterator i;

	for(i=custom_positions.begin(); i!=custom_positions.end(); i++)
		if(*i == p)
			return;
	custom_positions.push_back(p);
}

void CMesh::AddAnimation(CAnimation *a)
{
	vector<CAnimation *>::iterator i;

	for(i=animations.begin(); i!=animations.end(); i++)
		if(*i == a)
			return;
	animations.push_back(a);
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
}

void CMesh::RemoveCustomPosition(CCustomPosition *p)
{
	vector<CCustomPosition *>::iterator i;

	for(i=custom_positions.begin(); i!=custom_positions.end(); i++)
		if(*i == p)
		{
			custom_positions.erase(i);
			break;
		}
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
}

void CMesh::SetIDs(void)
{
	int i;
	vector<CVertex *>::iterator v;
	vector<CAnimation *>::iterator a;
	CKeyFrame *f;
	vector<CCustomPosition *>::iterator p;

	for(v=vertices.begin(), i=0; v!=vertices.end(); v++, i++)
		SetVertexId((*v), i);

	for(a=animations.begin(); a!=animations.end(); a++)
		for(f=(*a)->key_first; f; f=f->chain_next, i++)
			f->id = i;

	for(p=custom_positions.begin(), i=0; p!=custom_positions.end(); p++)
		(*p)->id = i;
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
}

void CMesh::InvalidateVertex(CVertex *v)
{
	if(outdated_vertices.find(v) != outdated_vertices.end())
		return;
	outdated_vertices.insert(v);
}

void CMesh::AssignVertexArrayPositions(void)
{
	vector<CVertex *>::iterator i;
	int c;

	for(c=0, i=vertices.begin(); i!=vertices.end(); c++, i++)
		(*i)->index = c;
}

void CMesh::RefreshVBO(void)
{
	vector<CTriangle *>::iterator i;
	vector<CVertex *>::iterator v;
	vector<CMaterial *>::iterator m;
	CVertex *vt;
	CTriangle *t;
	CMaterial *mt;
	int c;
	int j;
	int d;
	int m_max;
	float *vertex_data;
	float *normal_data;
	float *face_normal_data;
	float *uvcoord_data;

	data_count = GetVertexCount();
	vertex_vbo->SetSize(data_count);
	normal_vbo->SetSize(data_count);
	face_normal_vbo->SetSize(data_count);
	uvcoord_vbo->SetSize(data_count);

	vertex_data = vertex_vbo->GetData();
	normal_data = normal_vbo->GetData();
	face_normal_data = face_normal_vbo->GetData();
	uvcoord_data = uvcoord_vbo->GetData();

	m_max = GetMaterialCount();
	if(mat_data_count != m_max)
	{
		mat_data_count = m_max;
		if(mat_indices)
		{
			delete [] mat_indices;
			delete [] mat_list;
		}
		mat_indices = new int[m_max];
		mat_list = new CMaterial *[m_max];
	}

	/*while(!outdated_vertices.empty())
	{
		vt = *outdated_vertices.begin();
		outdated_vertices.erase(outdated_vertices.begin());
		c = vt->index;
		d = c*3;
		memcpy(vertex_data + d, vt->v, 3 * sizeof(float));
		memcpy(normal_data + d, vt->normal.v, 3 * sizeof(float));
		memcpy(face_normal_data + d, vt->normal.v, 3 * sizeof(float));
		memcpy(uvcoord_data + c*2, vt->uv.v, 2 * sizeof(float));
	}*/

	for(v=vertices.begin(), c=0; v!=vertices.end(); v++, c++)
	{
		vt = *v;
		vt->index = c;
		d = c*3;
		memcpy(vertex_data + d, vt->v, 3 * sizeof(float));
		memcpy(normal_data + d, vt->normal.v, 3 * sizeof(float));
		memcpy(face_normal_data + d, vt->normal.v, 3 * sizeof(float));
		memcpy(uvcoord_data + c*2, vt->uv.v, 2 * sizeof(float));

	}

	for(m=materials.begin(); m!=materials.end(); m++)
		(*m)->triangle_count = (*m)->triangle_i = 0;

	for(i=triangles.begin(); i!=triangles.end(); i++)
	{
		(*i)->mat->triangle_count++;
	}

	vao->Bind();
	for(m=materials.begin(); m!=materials.end(); m++)
	{
		mt = *m;
		mt->ibo->SetSize(mt->triangle_count * 3);
		mt->ibo->AssignData();
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

	vao->Bind();
	vertex_vbo->SetAttribute(face_shader.vertex_attribute, GL_FLOAT);
	normal_vbo->SetAttribute(face_shader.normal_attribute, GL_FLOAT);
	face_normal_vbo->SetAttribute(face_shader.face_normal_attribute, GL_FLOAT);
	uvcoord_vbo->SetAttribute(face_shader.uvcoord_attribute, GL_FLOAT);
}

void CMesh::PutToGL(CVector cam, int both_sides)
{
	vector<CMaterial *>::iterator i;

	//Sort(cam);
	SortMaterials();

	RefreshVBO();

	face_shader.SetMode(shader_enabled);
	face_shader.SetTwoSide(both_sides);
	face_shader.SetTransformation(CMesh::transformation);
	face_shader.SetDiffuseColor2(Vec(color[0], color[1], color[2]), color[3]);

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


int CMesh::LoadFromFileHandle_1_0(int h, const char *path, int no_material)
{
	float v[15];
	char tx[5][100];
	char name[100];
	char modus, read_modus;
	int ret = 0;
	int id;
	int i;
	int tvert[3];
	int counter = 0;
	int vp_number;
	union
	{
		float time;
		float len;
	};
	int *vp_id;
	char *tex_temp[6];
	CVector *vec;
	float *vp_coord;
	int rel_counter;
	int vp_count;
	CCustomPosition *created_p;
	CAnimation *created_a;
	CVertex *created_v;

	read_modus = READ_MODUS_READ;

	Create();
	
	while(1)
	{
	    if(read_modus == READ_MODUS_READ)
	    {
	        ret = read(h, &modus, 1);
	        read_modus = READ_MODUS_VALUES;
	
	        counter = 0;
	    }
	
	    if(read_modus == READ_MODUS_VALUES)
	    {
	    	switch(modus)
	    	{
	    		case FILE_MODUS_MAT: // read material
	    			if(counter < 5)
	            		    ret = read(h, &tx[counter], 100);
	            		else if(counter >= 5 && counter < 8)
	    				ret = read(h, &v[counter - 5], sizeof(float));
	            		else if(counter == 8)
	            		    	ret = read(h, &name, 100);
	            		else
	            		    read_modus = READ_MODUS_FINISH;
	    			break;



	    		case FILE_MODUS_VERT: // read vertex
	    			if(counter < 3)
	    				ret = read(h, &v[counter], sizeof(float));
	    			else if(counter == 3)
	    				ret = read(h, &id, sizeof(int));
	    			else if(counter > 3)
	    				read_modus = READ_MODUS_FINISH;
	    			break;



	    		case FILE_MODUS_TRI: // read triangle
	                    	if(counter < 3) // Vertices
	                    		ret = read(h, &tvert[counter], sizeof(int));
	    	                else if(counter >= 3 && counter < 15) // Other Vectors
	    				ret = read(h, &v[counter], sizeof(float));
	    			else if(counter == 15) // Material
	            	        	ret = read(h, &name, 100);
	                    	else if(counter > 15)
	                        		read_modus = READ_MODUS_FINISH;
	    			break;



			case FILE_MODUS_POS: // read position
				if(counter == 0)
				{
					ret = read(h, &name, 100);
				}
				else if(counter == 1)
				{
					ret = read(h, &vp_number, sizeof(int));
					vp_id = new int[vp_number];
					vp_coord = new float[vp_number * 3];
					vp_count = 0;
				}
				else if(counter > 1 && counter < (vp_number * 4) + 2)
				{
					rel_counter = counter - 2;
					vp_count = (rel_counter - (rel_counter % 4)) / 4;
					if(rel_counter % 4 == 0)
						ret = read(h, &vp_id[vp_count], sizeof(int));
					else
						ret = read(h, &vp_coord[vp_count * 3 + (rel_counter % 4) - 1], sizeof(float));
				}
				else if(counter >= (vp_number * 4) + 2)
				{
					read_modus = READ_MODUS_FINISH;
				}
				break;



			case FILE_MODUS_KEY: // read keyframe
				if(counter == 0)
					ret = read(h, &name, 100);
				else if(counter == 1)
					ret = read(h, &time, sizeof(float));
				else if(counter == 2)
				{
					ret = read(h, &vp_number, sizeof(int));
					vp_id = new int[vp_number];
					vp_coord = new float[vp_number * 3];
				}
				else if(counter > 2 && counter < (vp_number * 4) + 3)
				{
					rel_counter = counter - 3;
					vp_count = (rel_counter - (rel_counter % 4)) / 4;
					if(rel_counter % 4 == 0)
						ret = read(h, &vp_id[vp_count], sizeof(int));
					else
						ret = read(h, &vp_coord[vp_count * 3 + (rel_counter % 4) - 1], sizeof(float));
				}
				else if(counter >= (vp_number * 4) + 3)
					read_modus = READ_MODUS_FINISH;

				break;



			case FILE_MODUS_ANIM: // read animation
				if(counter == 0)
					ret = read(h, &name, 100);
				else if(counter == 1)
					ret = read(h, &len, sizeof(float));
				else if(counter > 1)
					read_modus = READ_MODUS_FINISH;
				break;
	    	}
	    }
	
	    if(modus == FILE_MODUS_END)
		    break;

	    if(ret < 1)
	    {
		    printf("unexpected eof\n");
		    break;
	    }
	
	    counter++;
	
	    if(read_modus == READ_MODUS_FINISH)
	    {
	    	switch(modus)
	    	{
	    		case FILE_MODUS_MAT:
				if(no_material)
					break;
	    			tex_temp[0] = new char[100];
	            		tex_temp[1] = new char[100];
	            		tex_temp[2] = new char[100];
	            		tex_temp[3] = new char[100];
				tex_temp[4] = new char[100];
				tex_temp[5] = new char[100];
	            		snprintf(tex_temp[0], 100, "%s", tx[0]);
	            		snprintf(tex_temp[1], 100, "%s", tx[1]);
	            		snprintf(tex_temp[2], 100, "%s", tx[2]);
				snprintf(tex_temp[3], 100, "%s", tx[3]);
				snprintf(tex_temp[4], 100, "%s", tx[4]);
	            		snprintf(tex_temp[5], 100, "%s", name);

	            		CreateMaterialRelative(path, tex_temp[0], tex_temp[1], v[0], tex_temp[2], tex_temp[3], v[1],
					       tex_temp[4], v[2], tex_temp[5]);
	    			break;
	    		case FILE_MODUS_VERT:
	    			created_v = CreateVertex(Vec(v[0], v[1], v[2]));
	    			SetVertexId(created_v, id);
	    			break;
	    		case FILE_MODUS_TRI:
	    			CreateTriangle(GetVertexByID(tvert[0]),   // 1
	                           		GetVertexByID(tvert[1]),       	// 2
	                           		GetVertexByID(tvert[2]),       	// 3
	                           		Vec(v[3], v[4], v[5]),     		// color
	                           		name,                    		// material
	                           		Vec(v[6], v[7], v[8]),    		// t1
	                           		Vec(v[9], v[10], v[11]),    		// t2
	                           		Vec(v[12], v[13], v[14]));   		// t3
	                    	break;
			case FILE_MODUS_POS: // create position
				created_p = new CCustomPosition(this, name);
				vec = new CVector[vp_number];
				for(i=0; i<vp_number; i++)
				{
					vec[i].x = vp_coord[i * 3 + 0];
					vec[i].y = vp_coord[i * 3 + 1];
					vec[i].z = vp_coord[i * 3 + 2];
				}
				created_p->CopyFromData(vp_number, vp_id, vec);
				break;

			case FILE_MODUS_KEY: // create keyframe
				created_a = GetAnimationByName(name);
				if(!created_a)
					break;
				vec = new CVector[vp_number];
				for(i=0; i<vp_number; i++)
				{
					vec[i].x = vp_coord[i * 3 + 0];
					vec[i].y = vp_coord[i * 3 + 1];
					vec[i].z = vp_coord[i * 3 + 2];
				}
				created_a->NewKeyFrameFromData(time, vp_number, vp_id, vec);
				break;

			case FILE_MODUS_ANIM: // create animation
				created_a = new CAnimation(this, name, len, 0);
				break;

	    	}
	
	        read_modus = READ_MODUS_READ;
	        counter = 0;
	    }
	}

	close(h);
	
	return 1;
}



int CMesh::LoadFromFile(const char *file, int no_material)
{
	int h;
	int r;
	char *path;
	char header[strlen(FILE_HEADER_MESH_1_0)];
	struct stat s;
	
	if(stat(file, &s) != 0 && errno == ENOENT)
	    return 0;
	
	path = PathOfFile(file);

	h = open(file, O_RDONLY);
	if(h < 0)
	    return 0;

	read(h, &header, strlen(FILE_HEADER_MESH_1_0));

	if(strcmp(header, FILE_HEADER_MESH_1_0) == 0)
	{
		r = LoadFromFileHandle_1_0(h, path, no_material);
	}
	else
	{
		close(h);
		r = LoadFromFile_xml(file, path, no_material);
	}	

	//CalculateNormalsSolid();
	AssignVertexArrayPositions();

	return r;
}

int TEMVersionFromString(const char *s)
{
	if(strcmp(s, TEM_VERSION_0_2_STRING) == 0)
		return TEM_VERSION_0_2;
	if(strcmp(s, TEM_VERSION_0_1_STRING) == 0)
		return TEM_VERSION_0_1;
	return -1;
}

int CMesh::LoadFromFile_xml(const char *file, const char *path, int no_material)
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
					version_string, file, TEM_VERSION_0_2_STRING);
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
		if(xmlStrcmp(cur->name, (const xmlChar *)"position") == 0)
			ParsePositionNode(cur);
		if(xmlStrcmp(cur->name, (const xmlChar *)"animation") == 0)
			ParseAnimationNode(cur);
		if(xmlStrcmp(cur->name, (const xmlChar *)"entity") == 0)
			ParseEntityNode(cur);


		//CallRefresh();

		cur = cur->next;
	}

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
	InvalidateVertex(r);
	SetVertexId(r, id);
	r->uv_set = file_version >= TEM_VERSION_0_2;
	r->uv = uv;
	r->normal = normal;
	return r;
}

CMaterial *CMesh::ParseMaterialNode(xmlNodePtr cur, const char *path)
{
	xmlNodePtr child;
	char *diffuse_file;
	char *specular_file; float exponent;
	char *normal_file;
	char *aeb_file; float bump_factor;
	char *height_file; float height_factor;
	char *name;
	xmlChar *temp;
	CMaterial *r;

	name = diffuse_file = specular_file = normal_file = aeb_file = height_file = 0;

	name = (char *)xmlGetProp(cur, (const xmlChar *)"name");
	child = cur->children;
	diffuse_file = cstr("$NONE");
	specular_file = cstr("$NONE");
	normal_file = cstr("$NONE");
	aeb_file = cstr("$NONE");
	height_file = cstr("$NONE");
	while(child)
	{
		if(!xmlStrcmp(child->name, (const xmlChar *)"diffuse"))
		{
			if((temp = xmlGetProp(child, (const xmlChar *)"file")))
				diffuse_file = (char *)temp;
		}
		if(!xmlStrcmp(child->name, (const xmlChar *)"specular"))
		{
			if((temp = xmlGetProp(child, (const xmlChar *)"file")))
				specular_file = (char *)temp;
			if((temp = xmlGetProp(child, (const xmlChar *)"exponent")))
				exponent = atof((const char *)temp);
		}
		if(!xmlStrcmp(child->name, (const xmlChar *)"normal"))
		{
			if((temp = xmlGetProp(child, (const xmlChar *)"file")))
				normal_file = (char *)temp;
		}
		if(!xmlStrcmp(child->name, (const xmlChar *)"aeb"))
		{
			if((temp = xmlGetProp(child, (const xmlChar *)"file")))
				aeb_file = (char *)temp;
			if((temp = xmlGetProp(child, (const xmlChar *)"factor")))
				bump_factor = atof((const char *)temp);
		}
		if(!xmlStrcmp(child->name, (const xmlChar *)"height"))
		{
			if((temp = xmlGetProp(child, (const xmlChar *)"file")))
				height_file = (char *)temp;
			if((temp = xmlGetProp(child, (const xmlChar *)"factor")))
				height_factor = atof((const char *)temp);
			else
				height_factor = 1.0;
		}
		child = child->next;
	}

	r = CreateMaterialRelative(path, diffuse_file, specular_file, exponent, normal_file, aeb_file, bump_factor, height_file, height_factor, name);
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

CCustomPosition *CMesh::ParsePositionNode(xmlNodePtr cur)
{
	char *name;
	xmlNodePtr child;
	int count;
	int i;
	int *vert;
	CVector *vec;
	CCustomPosition *r;

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

	r = new CCustomPosition(this, name);
	r->CopyFromData(count, vert, vec);
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
	xmlDocPtr doc;
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
	vector<CCustomPosition *>::iterator p;
	CVertexPosition *vp;
	vector<CAnimation *>::iterator a;
	CKeyFrame *k;
	vector<CEntity *>::iterator e;
	map<string, CEntityAttribute *>::iterator ea;

	SetIDs();
	ChangePosition(idle_position);

	doc = xmlNewDoc((const xmlChar *)"1.0");
	root = xmlNewNode(0, (const xmlChar *)"tmesh");
	xmlNewProp(root, (const xmlChar *)"version", (const xmlChar *)TEM_CURRENT_VERSION_STRING);
	xmlDocSetRootElement(doc, root);

	for(m=materials.begin(); m!=materials.end(); m++)
	{
		if(strcmp((*m)->name, "$NONE") == 0)
			continue;

		cur = xmlNewNode(0, (const xmlChar *)"material");
		xmlNewProp(cur, (const xmlChar *)"name", (const xmlChar *)(*m)->name);

		child = xmlNewNode(0, (const xmlChar *)"diffuse");
		xmlNewProp(child, (const xmlChar *)"file", (const xmlChar *)(*m)->diffuse_filename);
		xmlAddChild(cur, child);

		child = xmlNewNode(0, (const xmlChar *)"specular");
		xmlNewProp(child, (const xmlChar *)"file", (const xmlChar *)(*m)->specular_filename);
		xmlNewProp(child, (const xmlChar *)"exponent", (const xmlChar *)ftoa((*m)->specular_exponent));
		xmlAddChild(cur, child);

		child = xmlNewNode(0, (const xmlChar *)"normal");
		xmlNewProp(child, (const xmlChar *)"file", (const xmlChar *)(*m)->normal_filename);
		xmlAddChild(cur, child);

		child = xmlNewNode(0, (const xmlChar *)"aeb");
		xmlNewProp(child, (const xmlChar *)"file", (const xmlChar *)(*m)->aeb_filename);
		xmlNewProp(child, (const xmlChar *)"factor", (const xmlChar *)ftoa((*m)->bump_factor));
		xmlAddChild(cur, child);

		child = xmlNewNode(0, (const xmlChar *)"height");
		xmlNewProp(child, (const xmlChar *)"file", (const xmlChar *)(*m)->height_filename);
		xmlNewProp(child, (const xmlChar *)"factor", (const xmlChar *)ftoa((*m)->height_factor));
		xmlAddChild(cur, child);

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
		xmlNewProp(cur, (const xmlChar *)"name", (const xmlChar *)(*p)->name);
		for(vp=(*p)->pos_first; vp; vp=vp->chain_next)
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
		}
	}

	xmlDocDumpFormatMemory(doc, &buf, &buffer_size, 1);
	int r = xmlSaveFormatFile(file, doc, 1);
	xmlFree(buf);
	xmlFreeDoc(doc);
	return r;
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
		a_type = string((const char *)xmlGetProp(cur, (const xmlChar *)"type"));
		a_name = string((const char *)xmlGetProp(cur, (const xmlChar *)"name"));

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
			a->type = CEntityAttribute::VECTOR2;
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
		if((*i)->name.compare(group) == 0)
			r.insert(pair<string, CEntity *>((*i)->name, *i));

	return r;
}


CCustomPosition *CMesh::GetPositionByName(const char *name)
{
	vector<CCustomPosition *>::iterator p;

	for(p=custom_positions.begin(); p!=custom_positions.end(); p++)
		if(strcmp(name, (*p)->name) == 0)
			return *p;
	return 0;
}

CCustomPosition *CMesh::CreatePosition(const char *name)
{
	if(GetPositionByName(name))
		return 0;

	return new CCustomPosition(this, name);
}

void CMesh::ChangePosition(const char *name, const char *idle)
{
	CCustomPosition *p;

	if(strcmp(name, idle) == 0)
		current_position = idle_position;
	else if((p = GetPositionByName(name)) != 0)
		current_position = p;

	if(current_position)
		current_position->ApplyPosition();
}

void CMesh::ChangePosition(CMeshPosition *pos)
{
	current_position = pos;

	if(current_position)
		current_position->ApplyPosition();
}

CMeshPosition *CMesh::GetCurrentPosition(void)
{
	return current_position;
}

void CMesh::CopyPositionFromVertices(void)
{
	if(current_position)
		current_position->CopyFromVertices();
}


char *CMesh::GetCurrentPositionName(const char *idle)
{
	char *r;
	const char *t;

	if(!current_position)
		return 0;

	if(current_position == idle_position)
		t = idle;
	else
		t = ((CCustomPosition *)current_position)->name;

	r = new char[strlen(t) + 1];
	strcpy(r, t);
	return r;
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
	if(b->mat->tex->transparent == a->mat->tex->transparent)
		return a->mat < b->mat;
	else
		return b->mat->tex->transparent && !a->mat->tex->transparent;
}

bool CompareMaterialTransparency(CMaterial *a, CMaterial *b)
{
	if(b->tex->transparent == a->tex->transparent)
		return a < b;
	else
		return b->tex->transparent && !a->tex->transparent;
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
}

void CMesh::SortMaterials(void)
{
	sort(materials.begin(), materials.end(), CompareMaterialTransparency);
}



CMeshHandler::CMeshHandler(CMesh *mesh)
{
	this->mesh = mesh;
	animation = 0;
	position = 0;
	animation_mode = 0;
	loop = false;
}

void CMeshHandler::SetAnimation(const char *animation)
{
	CAnimation *a = mesh->GetAnimationByName(animation);
	if(a != this->animation)
		time = 0.0;
	this->animation = a;
}

void CMeshHandler::PlayAnimation(float time)
{
	if(!animation_mode || !animation)
		return;

	if(!loop)
		this->time = min(animation->GetLength(), this->time + time);
	else
		this->time = fmod(this->time + time, animation->GetLength());
}

bool CMeshHandler::GetAnimationFinished(void)
{
	if(!animation_mode || !animation)
		return true;

	return !loop && this->time >= animation->GetLength();
}

void CMeshHandler::SetPosition(const char *position)
{
	if(this->position)
		delete [] this->position;
	this->position = cstr(position);
}

void CMeshHandler::PutToGL(CVector cam)
{
	if(animation_mode && animation)
	{
		mesh->ChangeAnimation(animation);
		mesh->SetAnimationLoop(0);
		animation->SetTime(time);
		mesh->ApplyAnimation();
	}
	else
	{
		if(position)
			mesh->ChangePosition(position);
		else
			mesh->ChangePosition("Idle");
	}
	mesh->PutToGL(cam);
}
