#include "towerengine.h"

int TriangleNumber(CTriangle *first)
{
    CTriangle *t;
    int i;

    i=0;
    for(t=first; t; t=t->chain_next)
        i++;

    return i;
}



CTriangle::CTriangle(CMesh *mesh)
{
	v = new CVertex *[3];
    if(mesh)
    {
        this->mesh = mesh;
        mesh->AddTriangle(this);
    }
    else
    {
        mesh = 0;
    }

    mat = 0;
    m_name = new char[100];
    strcpy(m_name, "$NONE");
}

CTriangle::~CTriangle()
{
    if(!mesh)
        return;

    mesh->RemoveTriangle(this);
}

void CTriangle::SetMaterial(CVector v1, CVector v2, CVector v3, char material[100])
{
    tex_coord[0] = v1;
    tex_coord[1] = v2;
    tex_coord[2] = v3;
    mat = mesh->GetMaterialByName(material);
    if(!mat)
    	mat = mesh->GetIdleMaterial();
    if(!m_name)
        m_name = new char[100];
    strcpy(m_name, material);
}

void CTriangle::PutToGL(int wireframe, int both_sides)
{
	int lighting = 0;

	glColor4f(color.x, color.y, color.z, 1.0);

	if(!wireframe)
	{
		mat->PutToGL();

		if(both_sides)
			CEngine::GetFaceShader()->SetTwoSide(1);
		else
			CEngine::GetFaceShader()->SetTwoSide(0);


		glBegin(GL_TRIANGLES);
		CEngine::GetFaceShader()->SetTexCoord(tex_coord[0]);
		v[0]->PutToGL();
		CEngine::GetFaceShader()->SetTexCoord(tex_coord[1]);
		v[1]->PutToGL();
		CEngine::GetFaceShader()->SetTexCoord(tex_coord[2]);
		v[2]->PutToGL();
		glEnd();

		if(both_sides)
		{
			glBegin(GL_TRIANGLES);
			CEngine::GetFaceShader()->SetTexCoord(tex_coord[1]);
			v[1]->PutToGL();
			CEngine::GetFaceShader()->SetTexCoord(tex_coord[0]);
			v[0]->PutToGL();
			CEngine::GetFaceShader()->SetTexCoord(tex_coord[2]);
			v[2]->PutToGL();
			glEnd();
		}
	}
	else
	{
		if(glIsEnabled(GL_LIGHTING))
		{
			lighting = 1;
			glDisable(GL_LIGHTING);
		}
		UseNoShader();
		glBegin(GL_LINES);
		v[0]->PutToGL();
		v[1]->PutToGL();
		v[0]->PutToGL();
		v[2]->PutToGL();
		v[1]->PutToGL();
		v[2]->PutToGL();
		glEnd();
		if(lighting)
			glEnable(GL_LIGHTING);
	}
}

void CTriangle::Set(CVertex *v1, CVertex *v2, CVertex *v3, CVector color, CVector t1, CVector t2, CVector t3)
{
    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    this->color = color;
    tex_coord[0] = t1;
    tex_coord[0] = t2;
    tex_coord[0] = t3;
}

CTriangle *CTriangle::CreateTriangle(CVertex *v1, CVertex *v2, CVertex *v3, CVector color, char material[100], CVector t1, CVector t2, CVector t3, CMesh *chain)
{
    CTriangle *t;

    t = new CTriangle(chain);

    t->Create(v1, v2, v3, color);
    t->SetMaterial(t1, t2, t3, material);

    return t;
}

void CTriangle::CalculateNormalSolid(void)
{
	normal[0] = normal[1] = normal[2] = fnormal = v[0]->normal = v[1]->normal = v[2]->normal = CrossNormal();
}

int CTriangle::MaterialState(void)
{
    if(!mat)
        return 0;
    return 1;
}
