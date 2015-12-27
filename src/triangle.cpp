#include "towerengine.h"

using namespace std;

tTriangle::tTriangle(void)
{
    cam_dist = 0.0;
    mat = 0;
}

tTriangle::~tTriangle()
{
}

void tTriangle::SetMaterial(tMaterial *material)
{
    mat = material;
}

void tTriangle::Set(tVertex *v1, tVertex *v2, tVertex *v3, tVector color)
{
    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    this->color = color;
}

tTriangle *tTriangle::CreateTriangle(tVertex *v1, tVertex *v2, tVertex *v3, tVector color, tMaterial *material)
{
    tTriangle *t;

    t = new tTriangle();

    t->Create(v1, v2, v3, color);
    t->SetMaterial(material);
    t->CalculateTangents();

    return t;
}

void tTriangle::CalculateTangents(void)
{
	tVector edge1 = v[1]->pos - v[0]->pos;
	tVector edge2 = v[2]->pos - v[0]->pos;
	tVector2 uv_edge1 = v[1]->uv - v[0]->uv;
	tVector2 uv_edge2 = v[2]->uv - v[0]->uv;

	float r = uv_edge1.x * uv_edge2.y - uv_edge2.x * uv_edge1.y;

	if(r != 0.0)
	{
		r = 1.0 / r;

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

	v[0]->tang = v[1]->tang = v[2]->tang = tang;
	v[0]->bitang = v[1]->bitang = v[2]->bitang = bitang;
}
