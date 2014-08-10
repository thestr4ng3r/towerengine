#ifndef MESH_H
#define MESH_H

#define TEM_VERSION_0_1				1
#define TEM_VERSION_0_2				2
#define TEM_VERSION_0_3				3

#define TEM_VERSION_0_1_STRING		"0.1"
#define TEM_VERSION_0_2_STRING		"0.2"
#define TEM_VERSION_0_3_STRING		"0.3"

#define TEM_CURRENT_VERSION			TEM_VERSION_0_3
#define TEM_CURRENT_VERSION_STRING	TEM_VERSION_0_3_STRING

template <class T> class VBO;

class CMesh
{
	private:
		int file_version;

		bool wireframe;
		bool loop_anim;
		bool anim_finished;
		bool animation_mode;

		bool refresh_vbos;
		bool refresh_ibos;

		VAO *vao;
		VBO<float> *normal_vbo;
		VBO<float> *tang_vbo;
		VBO<float> *bitang_vbo;
		VBO<float> *face_normal_vbo;
		VBO<float> *uvcoord_vbo;
		int data_count;

		set<CVertex *> outdated_vertices;

		CMeshPose *current_pose;
		CAnimation *current_animation;

		CMeshPose *idle_pose;

		CMaterial *idle_material;

		vector<CVertex *> vertices;
		vector<CTriangle *> triangles;
		vector<CMaterial *> materials;
		map<string, CMeshPose *> custom_pose;
		vector<CAnimation *> animations;
		vector<CEntity *> entities;

		map<int, CVertex *> vertex_indices;

		void (*refresh_func)(void);

		void CallRefresh(void) { if(refresh_func != 0) (*refresh_func)(); }


		void RefreshAllVBOs(void);
		void DeleteVBOData(void);

		CVertex *ParseVertexNode(xmlNodePtr cur);
		CMaterial *ParseMaterialNode(xmlNodePtr cur, const char *path);
		CTriangle *ParseTriangleNode(xmlNodePtr cur);
		CMeshPose *ParsePoseNode(xmlNodePtr cur);
		CAnimation *ParseAnimationNode(xmlNodePtr cur);
		CKeyFrame *ParseKeyFrameNode(xmlNodePtr cur, CAnimation *anim);
		CEntity *ParseEntityNode(xmlNodePtr cur);

	public:
		static float transformation[16];
		static float color[4];
		static void LoadIdentity(void);
		static void Translate(CVector v);
		static void RotateX(float a);
		static void RotateY(float a);
		static void RotateZ(float a);
		static void SetXY(CVector x, CVector y);
		static void SetYZ(CVector y, CVector z);
		static void SetXZ(CVector x, CVector z);
		static void SetXYZ(CVector x, CVector y, CVector z);
		static void Rotate(CVector v);
		static void Scale(CVector v);
		static void Color(CVector c, float a = 1.0);
		void ApplyTransformation(void);
		void ApplyMatrix(float m[16]);

		#ifdef TMS_USE_LIB_G3D
        static G3DContext *g3d_context;
        static void CreateG3DContext(void) { if(g3d_context) return; g3d_context = g3d_context_new(); };
        static void DeleteG3DContext(void) { if(!g3d_context) return; g3d_context_free(g3d_context); g3d_context = 0; };
		#endif

       	int LoadFromFile_libg3d(const char *file);
		int MergeWithFile_libg3d(const char *file);

        int LoadFromFile_lib3ds(const char *file);
		int MergeWithFile_lib3ds(const char *file);
		bool LoadFromFile(const char *file, int no_material = 0);
		bool LoadFromFile_xml(const char *file, const char *path, int no_material);
        //int LoadFromFile_0_0(const char *file) { printf("LoadFromFile_0_0 function was removed.\n"); return 0; };
        int SaveToFile(const char *file);
 	    int SaveToFile_0_1(const char *file);
		int SaveToFile_old(const char *file);
		//void SetOrientation(CVector o);
		//CVector ApplyOrientation(CVector v) { return Orientation(v, orient_x, orient_y, orient_z); };
		void PutToGL(CVector cam = Vec(0.0, 0.0, 0.0), int both_sides = 0);
		int GetState(void);
		void Create(void);
		void Delete(void);

		void SetWireframe(int wf)					{ wireframe = wf ? 1 : 0; }
		int GetWireframe(void)						{ return wireframe; }

		int GetVertexCount(void)					{ return vertices.size(); }
		int GetTriangleCount(void)					{ return triangles.size(); }
		int GetMaterialCount(void)					{ return materials.size(); }
		int GetCustomPosesCount(void)				{ return custom_pose.size(); }
		int GetAnimationCount(void)					{ return animations.size(); }
		int GetEntityCount(void)					{ return entities.size(); }

		CVertex *GetVertex(int i)					{ return vertices.at(i); }
		CTriangle *GetTriangle(int i)				{ return triangles.at(i); }
		CMaterial *GetMaterial(int i)				{ return materials.at(i); }
		CMeshPose *GetCustomPose(string s) 			{ return custom_pose.at(s); }
		CAnimation *GetAnimation(int i)				{ return animations.at(i); }
		CEntity *GetEntity(int i)					{ return entities.at(i); }

		CMaterial *GetIdleMaterial(void)			{ return idle_material; }

		map<string, CEntity *> GetEntitiesInGroup(const char *group = "");

		void AddVertex(CVertex *v);
		void AddTriangle(CTriangle *t);
		void AddMaterial(CMaterial *m);
		void AddCustomPose(string name, CMeshPose *p);
		void AddAnimation(CAnimation *a);

		void RemoveVertex(CVertex *v);
		void RemoveTriangle(CTriangle *t);
		void RemoveMaterial(CMaterial *m);
		void RemoveCustomPose(string name);
		void RemoveAnimation(CAnimation *a);
		void RemoveEntity(CEntity *e);

		IBO *CreateIBO(void)						{ return new IBO(vao); }

		CMeshPose *GetIdlePose(void)		{ return idle_pose; }
		CMeshPose *GetCustomPoseByName(string name);
		CMeshPose *GetCurrentPose(void);
		string GetCurrentPoseName(string idle = string("Idle"));

		CAnimation *CreateAnimation(const char *name, float len = 1.0);
		void ChangeAnimation(CAnimation *a);
		void ChangeAnimation(const char *name);
		CAnimation *GetAnimationByName(const char *name);
		CAnimation *GetCurrentAnimation(void);
		char *GetCurrentAnimationName(void);
		void PlayAnimation(float t);
		void ApplyAnimation(void);
		void ResetAnimationFinished(void) { anim_finished = 0; };
		void SetAnimationLoop(int l) { loop_anim = l ? 1 : 0; anim_finished = 0; };
		int GetAnimationFinished(void) { return anim_finished; };

        void SetTriangleMaterials(void);
		void CalculateNormalsSolid(void);

		CVertex *CreateVertex(CVector v);
		CTriangle *CreateTriangle(CVertex *v1, CVertex *v2, CVertex *v3, CVector color, char material[100], CVector t1, CVector t2, CVector t3);
		CTriangle *CreateTriangleAuto(CVector v1, CVector v2, CVector v3, CVector color, char material[100], CVector t1, CVector t2, CVector t3);
		CMeshPose *CreateCustomPose(string name);
		CEntity *CreateEntity(string name, string group = string());

		VBO<float> *CreateFloatVBO(int components);
		void AssignVertexArrayPositions(void);

		void ChangePose(string name, string idle = "Idle");
		void ChangePose(CMeshPose *pos);
		void CopyPoseFromVertices(void);

		CMaterial *GetMaterialByName(string name);
		CVertex *GetVertexByID(int id);

		void SetVertexId(CVertex *v, int id);

		void TriggerAllVBOsRefresh(void)	{ refresh_vbos = true; }
		void TriggerIBOsRefresh(void)		{ refresh_ibos = true; }
		void RefreshIBOs(void);

		void SortTriangles(CVector cam);
		void SortMaterials(void);

		void SetIDs(void);

		void SetRefreshFunc(void (*func)(void)) { refresh_func = func; }

		CMesh(void);
		~CMesh(void);
};

#endif
















