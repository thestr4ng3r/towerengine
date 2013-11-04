#ifndef _POSE_H
#define _POSE_H

class CVertex;
class CMesh;


struct CMeshPose
{
	CMesh *mesh;

	VBO<float> *vbo;
	map<CVertex *, CVector> vertices;

	int id; // TODO: ID (nur zum saven) wo anders speichern (am besten in maps)
	
	CMeshPose(CMesh *mesh);
	virtual ~CMeshPose(void);

	void CopyFromVertices(void);
	void Clear(void);
	void CopyFromData(int c, int *vert, CVector *vec);
	int Count(void);
	void ApplyPoseToVertices(void);
	void ApplyMixedPoseToVertices(CMeshPose *o, float mix);
	void RefreshVBO(void);
};

#endif
