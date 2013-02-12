
#ifndef _TRIANGLE_H
#define _TRIANGLE_H

struct CTriangle
{
	CMesh *mesh;
	CVertex **v;
	CVector normal[3];
	CVector fnormal;
	CVector tang, bitang;
	CMaterial *mat;
	CVector color;
	char *m_name;

	float cam_dist;

	CTriangle(CMesh *mesh = 0);
	~CTriangle(void);
	void Create(CVertex *v1, CVertex *v2, CVertex *v3, CVector _color) { v[0] = v1; v[1] = v2; v[2] = v3; color = _color; };
	void SetMaterial(char material[100]);
	void CalculateNormalSolid(void);
	void CalculateTangents(void);
	CVector CrossNormal(void) { CVector n = Cross((CVector)*v[2] - (CVector)*v[1], (CVector)*v[0] - (CVector)*v[1]); n.Normalize(); return n; };
	void Set(CVertex *v1, CVertex *v2, CVertex *v3, CVector color);
	void Flip(void) { CVertex *temp = v[0]; v[0] = v[2]; v[2] = temp; };

	int MaterialState(void);

	static CTriangle *CreateTriangle(CVertex *v1, CVertex *v2, CVertex *v3, CVector color, char material[100], CVector t1, CVector t2, CVector t3, CMesh *parent = 0);
};

#endif
