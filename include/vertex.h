
#ifndef _VERTEX_H
#define _VERTEX_H

struct tVertex : public tVector
{
	tMesh *mesh;
	//CVertex *chain_next;
	int id;
	float cam_dist;
	tVector2 uv;
	bool uv_set;
	tVector normal;
	tVector tang, bitang;
	int normal_count; // only for calculating smooth normals

	int index;

	tVertex(tMesh *mesh = 0);
	tVertex(tVector v, tMesh *mesh = 0);
	~tVertex(void);
	void Create(tMesh *mesh);

	int GetTriangles(tTriangle **&t, int *&number, tTriangle *exclude = 0);

	void SetVector(tVector v) { x = v.x; y = v.y; z = v.z; };

	tVector operator=(tVertex v);
};

#endif
