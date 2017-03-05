#include "towerengine.h"

using namespace std;

tMeshPose::tMeshPose(tMesh *mesh)
{
	id = 0;
	this->mesh = mesh;
	//vbo = mesh->CreateFloatVBO(3);
}

tMeshPose::~tMeshPose(void)
{
	//delete vbo;
}

void tMeshPose::CopyFromVertices(void)
{
	tVector p;
	tVertexIndex i;

	for(i=0; i<mesh->GetVertexCount(); i++)
	{
		tVertex &v = mesh->GetVertex(i);
		p = v.pos;
		vertices.insert(pair<tVertexIndex, tVector>(i, p));
	}
}

void tMeshPose::Clear(void)
{
	vertices.clear();
}

int tMeshPose::Count(void)
{
	return vertices.size();
}

void tMeshPose::CopyFromData(int c, tVertexIndex *vert, tVector *vec)
{
	int i;
	for(i=0; i<c; i++)
		vertices.insert(pair<tVertexIndex, tVector>(vert[i], vec[i]));
}


void tMeshPose::ApplyPoseToVertices(void)
{
	map<tVertexIndex, tVector>::iterator i;

	for(i=vertices.begin(); i!=vertices.end(); i++)
		mesh->GetVertex(i->first).pos = i->second;
}

void tMeshPose::ApplyMixedPoseToVertices(tMeshPose *o, float mix)
{
	map<tVertexIndex, tVector>::iterator i, oi;

	for(i=vertices.begin(); i!=vertices.end(); i++)
	{
		oi = o->vertices.find(i->first);
		tVertex &v = mesh->GetVertex(i->first);
		if(oi == o->vertices.end())
			v.pos = i->second;
		else
			v.pos = i->second * mix + oi->second * (1.0 - mix);

	}
}


void tMeshPose::RefreshVBO(tVBO<float> *vbo)
{
	vector<tVertex *>::iterator v;
	float *p;
	map<tVertexIndex, tVector>::iterator vi;
	float *vertex_data;
	tVertexIndex i;

	//mesh->AssignVertexArrayPositions();

	vbo->SetSize(mesh->GetVertexCount());
	vertex_data = vbo->GetData();

	for(i=0; i<mesh->GetVertexCount(); i++)
	{
		tVertex &vt = mesh->GetVertex(i);
		vi = vertices.find(i);
		if(vi==vertices.end())
			p = vt.pos.v;
		else
			p = vi->second.v;
		memcpy(vertex_data + i * 3, p, 3 * sizeof(float));
	}

	vbo->AssignData();
}













