
#ifndef _TRIANGLE_H
#define _TRIANGLE_H

struct tTriangle
{
	tVertex *v[3];
	tVector normal[3];
	tVector fnormal;
	tVector tang, bitang;
	tMaterial *mat;
	tVector color;

	float cam_dist;

	tTriangle(void);
	~tTriangle(void);
	void Create(tVertex *v1, tVertex *v2, tVertex *v3, tVector _color) { v[0] = v1; v[1] = v2; v[2] = v3; color = _color; };
	void SetMaterial(tMaterial *material);
	void CalculateTangents(void);
	void Set(tVertex *v1, tVertex *v2, tVertex *v3, tVector color);
	void Flip(void) { tVertex *temp = v[0]; v[0] = v[2]; v[2] = temp; };

	static tTriangle *CreateTriangle(tVertex *v1, tVertex *v2, tVertex *v3, tVector color, tMaterial *material);
};

#endif
