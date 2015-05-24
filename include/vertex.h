
#ifndef _VERTEX_H
#define _VERTEX_H

struct tVertex
{
	tMesh *mesh;

	int id;
	tVector pos;
	float cam_dist;
	tVector2 uv;
	bool uv_set;
	tVector normal;
	tVector tang, bitang;

	int index;

	tVertex(tMesh *mesh = 0);
	tVertex(tVector v, tMesh *mesh = 0);
	~tVertex(void);
	void Create(tMesh *mesh);

	int GetTriangles(tTriangle **&t, int *&number, tTriangle *exclude = 0);
};

#endif
