#ifndef _POSE_H
#define _POSE_H

struct tVertex;
class tMesh;


struct tMeshPose
{
	tMesh *mesh;

	//tVBO<float> *vbo;
	std::map<tVertexIndex, tVector> vertices;

	int id; // TODO: ID (nur zum saven) wo anders speichern (am besten in maps)
	
	tMeshPose(tMesh *mesh);
	virtual ~tMeshPose(void);

	void CopyFromVertices(void);
	void Clear(void);
	void CopyFromData(int c, tVertexIndex *vert, tVector *vec);
	int Count(void);
	void ApplyPoseToVertices(void);
	void ApplyMixedPoseToVertices(tMeshPose *o, float mix);
	void RefreshVBO(tVBO<float> *vbo);
};

#endif
