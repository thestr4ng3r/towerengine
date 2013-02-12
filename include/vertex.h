
#ifndef _VERTEX_H
#define _VERTEX_H

struct CVertex : public CVector
{
	CMesh *mesh;
	//CVertex *chain_next;
	int id;
	float cam_dist;
	CVector2 uv;
	bool uv_set;
	CVector normal;
	CVector tang, bitang;
	int normal_count; // only for calculating smooth normals

	int index;

	CVertex(CMesh *mesh = 0);
	CVertex(CVector v, CMesh *mesh = 0);
	~CVertex(void);
	void Create(CMesh *mesh);

	int GetTriangles(CTriangle **&t, int *&number, CTriangle *exclude = 0);

	void SetVector(CVector v) { x = v.x; y = v.y; z = v.z; };

	CVector operator=(CVertex v);
};

#endif
