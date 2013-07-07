#include "towerengine.h"

/*CVertexPosition::CVertexPosition(CMeshPosition *parent, CVertex *v, CVector p)
{
	this->parent = parent;
	chain_next = parent->pos_first;
	parent->pos_first = this;

	this->v = v;
	this->p = p;
}

CVertexPosition::~CVertexPosition(void)
{
}

void CVertexPosition::CopyPosition(void)
{
	p = (CVector)*v;
}

void CVertexPosition::ApplyPosition(void)
{
	v->SetVector(p);
}

void CVertexPosition::ApplyMixedPosition(CVector o, float mix)
{
	v->SetVector(Mix(p, o, mix));
}*/

//------------------------------------------------------------


CMeshPose::CMeshPose(CMesh *mesh)
{
	this->mesh = mesh;
}

CMeshPose::~CMeshPose(void)
{
	if(mesh->GetCurrentPose() == this)
		mesh->ChangePose(mesh->GetIdlePose());
}

void CMeshPose::CopyFromVertices(void)
{
	CVertex *v;
	CVector p;
	int i;

	for(i=0; i<mesh->GetVertexCount(); i++)
	{
		v = mesh->GetVertex(i);
		p = *v;
		vertices.insert(pair<CVertex *, CVector>(v, p));
	}
}

void CMeshPose::Clear(void)
{
	vertices.clear();
}

int CMeshPose::Count(void)
{
	return vertices.size();
}

void CMeshPose::CopyFromData(int c, int *vert, CVector *vec)
{
	int i;//, vi;
	CVertex *v;

	for(i=0; i<c; i++)
	{
		v = mesh->GetVertexByID(vert[i]);
		if(!v)
			continue;
		vertices.insert(pair<CVertex *, CVector>(v, vec[i]));
	}
}


void CMeshPose::ApplyPose(void)
{
	map<CVertex *, CVector>::iterator i;

	for(i=vertices.begin(); i!=vertices.end(); i++)
		i->first->SetVector(i->second);

	mesh->TriggerVertexVBORefresh();
}

void CMeshPose::ApplyMixedPose(CMeshPose *o, float mix)
{
	map<CVertex *, CVector>::iterator i, oi;

	for(i=vertices.begin(); i!=vertices.end(); i++)
	{
		oi = o->vertices.find(i->first);
		if(oi == o->vertices.end())
			i->first->SetVector(i->second);
		else
			i->first->SetVector(i->second * mix + oi->second * (1.0 - mix));

	}

	mesh->TriggerVertexVBORefresh();
}














