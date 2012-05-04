#ifndef _POSITION_H
#define _POSITION_H

class CVertex;
class CMesh;

struct CVertexPosition
{
	CMeshPosition *parent;
	CVertexPosition *chain_next;

	CVertex *v;
	CVector p;

	CVertexPosition(CMeshPosition *parent, CVertex *v, CVector p = Vec(0.0, 0.0, 0.0));
	~CVertexPosition(void);

	void SetName(const char *name);

	void CopyPosition(void);
	void ApplyPosition(void);
	void ApplyMixedPosition(CVector o, float mix);
	void ApplyMixedPosition(CVertexPosition *o, float mix) { ApplyMixedPosition(o->p, mix); };
};

struct CMeshPosition
{
	CMesh *mesh;

	CVertexPosition *pos_first;

	int id;
	
	CMeshPosition(CMesh *mesh);
	virtual ~CMeshPosition(void);

	void CopyFromVertices(void);
	void CleanUp(void);
	void Clear(void);
	void CopyFromData(int c, int *vert, CVector *vec);
	int Count(void);
	void ApplyPosition(void);

	virtual CMeshPosition *Copy(CMesh *m);
};

struct CIdlePosition : CMeshPosition
{
	CIdlePosition(CMesh *mesh);
	~CIdlePosition(void);

	virtual CIdlePosition *Copy(CMesh *m);
};

struct CCustomPosition : CMeshPosition
{
	CCustomPosition *chain_next;
	char *name;

	CCustomPosition(CMesh *mesh, const char *name);
	~CCustomPosition(void);

	void SetName(const char *name);

	virtual CCustomPosition *Copy(CMesh *m);
};

#endif
