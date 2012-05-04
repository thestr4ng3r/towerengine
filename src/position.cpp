#include "towerengine.h"


CVertexPosition::CVertexPosition(CMeshPosition *parent, CVertex *v, CVector p)
{
	this->parent = parent;
	chain_next = parent->pos_first;
	parent->pos_first = this;

	this->v = v;
	this->p = p;
}

CVertexPosition::~CVertexPosition(void)
{
	CVertexPosition *v;

	if(parent->pos_first == this)
		parent->pos_first = chain_next;
	else
		for(v=parent->pos_first; v; v=v->chain_next)
			if(v->chain_next == this)
			{
				v->chain_next = chain_next;
				break;
			}
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
}

//------------------------------------------------------------


CMeshPosition::CMeshPosition(CMesh *mesh)
{
	this->mesh = mesh;
	pos_first = 0;
}

CMeshPosition::~CMeshPosition(void)
{
	while(pos_first)
		delete pos_first;

	if(mesh->GetCurrentPosition() == this)
		mesh->ChangePosition(mesh->idle_position);
}

void CMeshPosition::CopyFromVertices(void)
{
	CVertex *v;
	CVertexPosition *vp;
	int c;
	int i;

	CleanUp();

	for(i=0; i<mesh->GetVertexCount(); i++)
	{
		v = mesh->GetVertex(i);
		c = 0;
		for(vp = pos_first; vp; vp=vp->chain_next)
			if(vp->v == v)
			{
				vp->CopyPosition();;
				c = 1;
				break;
			}
		if(c)
			continue;
		vp = new CVertexPosition(this, v, (CVector)*v);
	}
}

void CMeshPosition::CleanUp(void)
{
	CVertexPosition *vp, *t;
	CVertex *v;
	int i;

	int del;

	vp = pos_first;
	while(1)
	{
		if(!vp)
			break;

		del = 1;
		for(i=0; i<mesh->GetVertexCount(); i++)
		{
			v = mesh->GetVertex(i);
			if(vp->v == v)
			{
				del = 0;
				break;
			}
		}
		if(del)
		{
			t = vp->chain_next;
			delete vp;
			vp = t;
		}
		else
			vp = vp->chain_next;

	}

}

void CMeshPosition::Clear(void)
{
	while(pos_first)
		delete pos_first;
}

int CMeshPosition::Count(void)
{
	int i;
	CVertexPosition *p;

	i = 0;
	for(p=pos_first; p; p=p->chain_next)
		i++;
	return i;
}

void CMeshPosition::CopyFromData(int c, int *vert, CVector *vec)
{
	int i;//, vi;
	CVertex *v;

	for(i=0; i<c; i++)
	{
		//vi=0;
		//while(1)
		//{
		v = mesh->GetVertexByID(vert[i]); //, vi);
		if(!v)
			continue;
		new CVertexPosition(this, v, vec[i]);
			//vi++;
	//	}
	}
}


void CMeshPosition::ApplyPosition(void)
{
	CVertexPosition *vp;

	for(vp = pos_first; vp; vp = vp->chain_next)
	{
		vp->ApplyPosition();
	}
}

CMeshPosition *CMeshPosition::Copy(CMesh *m)
{
	CMeshPosition *r;

	r = new CMeshPosition(m);
	
	CVertexPosition *p;
	CVertex *v;
	for(p=pos_first; p; p=p->chain_next)
	{
		v = m->GetVertexByID(p->v->id);
		if(!v)
			continue;
		new CVertexPosition(r, v, p->p);
	}

	return r;
}

//------------------------------------------------------------

CIdlePosition::CIdlePosition(CMesh *mesh) : CMeshPosition(mesh)
{
	mesh->idle_position = this;
}

CIdlePosition::~CIdlePosition(void)
{
	mesh->idle_position = 0;
}

CIdlePosition *CIdlePosition::Copy(CMesh *m)
{
	CIdlePosition *r;

	r = new CIdlePosition(m);
	
	CVertexPosition *p;
	CVertex *v;
	for(p=pos_first; p; p=p->chain_next)
	{
		v = m->GetVertexByID(p->v->id);
		if(!v)
			continue;
		new CVertexPosition(r, v, p->p);
	}

	return r;
}


//------------------------------------------------------------

CCustomPosition::CCustomPosition(CMesh *mesh, const char *name) : CMeshPosition(mesh)
{
	mesh->AddCustomPosition(this);

	SetName(name);
}

CCustomPosition::~CCustomPosition(void)
{
	mesh->RemoveCustomPosition(this);
}

void CCustomPosition::SetName(const char *name)
{
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
}


CCustomPosition *CCustomPosition::Copy(CMesh *m)
{
	CCustomPosition *r;

	r = new CCustomPosition(m, name);
	
	CVertexPosition *p;
	CVertex *v;
	for(p=pos_first; p; p=p->chain_next)
	{
		v = m->GetVertexByID(p->v->id);
		if(!v)
			continue;
		new CVertexPosition(r, v, p->p);
	}

	return r;
}

