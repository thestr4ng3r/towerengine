
#ifndef _VERTEX_H
#define _VERTEX_H

struct tVertex
{
	int id;
	tVector pos;
	tVector2 uv;

	tVector normal;
	tVector tang, bitang;

	int index;

	tVertex(void);
	tVertex(tVector v);
	~tVertex(void);
};

#endif
