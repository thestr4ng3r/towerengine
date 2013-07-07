#include "towerengine.h"


CVertex::CVertex(CMesh *mesh) : CVector()
{
	Create(mesh);
}

CVertex::CVertex(CVector v, CMesh *mesh)
{
	Create(mesh);
	x = v.x;
	y = v.y;
	z = v.z;
}

void CVertex::Create(CMesh *mesh)
{
	if(mesh)
	{
		this->mesh = mesh;
		mesh->AddVertex(this);
		id = 0;
	}
	else
	{
		mesh = 0;
		id = 0;
	}
}

CVertex::~CVertex(void)
{
	CTriangle **t;
	int *n;
	int c, i;

	c = GetTriangles(t, n);
	for(i=0; i<c; i++)
		delete t[i];

	if(mesh)
        mesh->RemoveVertex(this);
}

int CVertex::GetTriangles(CTriangle **&t, int *&number, CTriangle *exclude)
{
	CTriangle *ti;
	int i;
	int count, j, k;

	count = 0;

	if(!mesh)
		return 0;

	for(i=0; i<mesh->GetTriangleCount(); i++)
	{
		ti = mesh->GetTriangle(i);

		if(ti == exclude)
			continue;

		for(j=0; j<3; j++)
			if(ti->v[j] == this)
			{
				count++;
				break;
			}
	}

	t = new CTriangle *[count];
	number = new int[count];
	k = 0;
	for(i=0; i<mesh->GetTriangleCount(); i++)
	{
		ti = mesh->GetTriangle(i);

		if(exclude == ti)
			continue;
		for(j=0; j<3; j++)
			if(ti->v[j] == this)
			{
				t[k] = ti;
				number[k] = j;
				k++;
				break;
			}
	}
	return count;
}

CVector CVertex::operator=(CVertex v)
{
	return (CVector)v;
}
