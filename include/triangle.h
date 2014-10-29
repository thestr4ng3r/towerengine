
#ifndef _TRIANGLE_H
#define _TRIANGLE_H

struct tTriangle
{
	tMesh *mesh;
	tVertex **v;
	tVector normal[3];
	tVector fnormal;
	tVector tang, bitang;
	tMeshMaterial *mat;
	tVector color;
	char *m_name;

	float cam_dist;

	tTriangle(tMesh *mesh = 0);
	~tTriangle(void);
	void Create(tVertex *v1, tVertex *v2, tVertex *v3, tVector _color) { v[0] = v1; v[1] = v2; v[2] = v3; color = _color; };
	void SetMaterial(char material[100]);
	void CalculateNormalSolid(void);
	void CalculateTangents(void);
	tVector CrossNormal(void) { tVector n = Cross((tVector)*v[2] - (tVector)*v[1], (tVector)*v[0] - (tVector)*v[1]); n.Normalize(); return n; };
	void Set(tVertex *v1, tVertex *v2, tVertex *v3, tVector color);
	void Flip(void) { tVertex *temp = v[0]; v[0] = v[2]; v[2] = temp; };

	int MaterialState(void);

	static tTriangle *CreateTriangle(tVertex *v1, tVertex *v2, tVertex *v3, tVector color, char material[100], tVector t1, tVector t2, tVector t3, tMesh *parent = 0);
};

#endif
