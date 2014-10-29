#ifndef _POSE_H
#define _POSE_H

class tVertex;
class tMesh;


struct tMeshPose
{
	tMesh *mesh;

	tVBO<float> *vbo;
	map<tVertex *, tVector> vertices;

	int id; // TODO: ID (nur zum saven) wo anders speichern (am besten in maps)
	
	tMeshPose(tMesh *mesh);
	virtual ~tMeshPose(void);

	void CopyFromVertices(void);
	void Clear(void);
	void CopyFromData(int c, int *vert, tVector *vec);
	int Count(void);
	void ApplyPoseToVertices(void);
	void ApplyMixedPoseToVertices(tMeshPose *o, float mix);
	void RefreshVBO(void);
};

#endif
