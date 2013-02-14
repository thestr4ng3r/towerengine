#include "towerengine.h"


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

    cam_dist = 0.0;
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

void CTriangle::SetMaterial(char material[100])
{
    mat = mesh->GetMaterialByName(string(material));
    if(!mat)
    	mat = mesh->GetIdleMaterial();
    if(!m_name)
        m_name = new char[100];
    strcpy(m_name, material);
}

void CTriangle::Set(CVertex *v1, CVertex *v2, CVertex *v3, CVector color)
{
    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    this->color = color;
}

CTriangle *CTriangle::CreateTriangle(CVertex *v1, CVertex *v2, CVertex *v3, CVector color, char material[100], CVector t1, CVector t2, CVector t3, CMesh *chain)
{
    CTriangle *t;

    t = new CTriangle(chain);

    t->Create(v1, v2, v3, color);
    t->SetMaterial(material);
    t->CalculateTangents();

    return t;
}

void CTriangle::CalculateNormalSolid(void)
{
	v[0]->normal = v[1]->normal = v[2]->normal = CrossNormal();
	// normal[0] = normal[1] = normal[2] = fnormal =
}


/*void ComputeTangentBasis(const Vec3& P1, const Vec3& P2, const Vec3& P3, const Vec2& UV1, const Vec2& UV2, const Vec2& UV3, Vec3 &tangent, Vec3 &bitangent )
{
   Vec3 Edge1 = P2 - P1;
   Vec3 Edge2 = P3 - P1;
   Vec2 Edge1uv = UV2 - UV1;
   Vec2 Edge2uv = UV3 - UV1;

   float cp = Edge1uv.y * Edge2uv.x - Edge1uv.x * Edge2uv.y;

   if ( cp != 0.0f ) {
      float mul = 1.0f / cp;
      tangent   = (Edge1 * -Edge2uv.y + Edge2 * Edge1uv.y) * mul;
      bitangent = (Edge1 * -Edge2uv.x + Edge2 * Edge1uv.x) * mul;

      tangent.Normalize();
      bitangent.Normalize();
   }
}*/

void CTriangle::CalculateTangents(void)
{
	CVector2 uv_edge1 = v[1]->uv - v[0]->uv;
	CVector2 uv_edge2 = v[2]->uv - v[0]->uv;
	CVector edge1 = *v[1] - *v[0];
	CVector edge2 = *v[2] - *v[0];

	float b = uv_edge1.y * uv_edge2.x - uv_edge1.x * uv_edge2.y;

	if(b != 0.0f)
	{
		float m = 1.0 / b;
		tang = (edge1 * -uv_edge2.y + edge2 * uv_edge1.y) * m;
		bitang = (edge1 * -uv_edge2.x + edge2 * uv_edge1.x) * m;
	}
	else
	{
		tang = edge1;
		bitang = edge2;
	}

	tang.Normalize();
	bitang.Normalize();

	/*tang =         (v2 * p1 - v1 * p2) *
			(1.0 / (u1 * v2 - v1 * u2));

	bitang =      (u2 * p1 - u1 * p2) *
		   (1.0 / (v1 * u2 - u1 * v2));*/

	v[0]->tang = v[1]->tang = v[2]->tang = tang;
	v[0]->bitang = v[1]->bitang = v[2]->bitang = bitang;
}

int CTriangle::MaterialState(void)
{
    if(!mat)
        return 0;
    return 1;
}
