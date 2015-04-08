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

template <class T> class tVBO;

// rapidxml forward declarations
namespace rapidxml
{
    template<class Ch> class xml_node;
    template<class Ch> class xml_attribute;
    template<class Ch> class xml_document;
}

class tMesh
{
	private:
		int file_version;

		bool wireframe;
		bool loop_anim;
		bool anim_finished;
		bool animation_mode;

		bool refresh_vbos;
		bool refresh_ibos;

		tVAO *vao;
		tVBO<float> *normal_vbo;
		tVBO<float> *tang_vbo;
		tVBO<float> *bitang_vbo;
		tVBO<float> *face_normal_vbo;
		tVBO<float> *uvcoord_vbo;
		int data_count;

		tBoundingBox bounding_box;

		set<tVertex *> outdated_vertices;

		tMeshPose *current_pose;
		tAnimation *current_animation;

		tMeshPose *idle_pose;

		tMeshMaterial *idle_material;

		vector<tVertex *> vertices;
		vector<tTriangle *> triangles;
		vector<tMeshMaterial *> materials;
		map<string, tMeshPose *> custom_pose;
		vector<tAnimation *> animations;
		vector<tEntity *> entities;

		map<int, tVertex *> vertex_indices;


		btTriangleMesh *physics_triangle_mesh;

		void (*refresh_func)(void);

		void CallRefresh(void) { if(refresh_func != 0) (*refresh_func)(); }


		void RefreshAllVBOs(void);
		void DeleteVBOData(void);

		tVertex *ParseVertexNode(rapidxml::xml_node<char> *cur);
		tMeshMaterial *ParseMaterialNode(rapidxml::xml_node<char> *cur, string path);
		tTriangle *ParseTriangleNode(rapidxml::xml_node<char> *cur);
		tMeshPose *ParsePoseNode(rapidxml::xml_node<char> *cur);
		tAnimation *ParseAnimationNode(rapidxml::xml_node<char> *cur);
		tKeyFrame *ParseKeyFrameNode(rapidxml::xml_node<char> *cur, tAnimation *anim);
		tEntity *ParseEntityNode(rapidxml::xml_node<char> *cur);

	public:
		static float color[4];

		static void Color(tVector c, float a = 1.0);

		void ApplyMatrix(float m[16]);

		#ifdef TMS_USE_LIB_G3D
        static G3DContext *g3d_context;
        static void CreateG3DContext(void) { if(g3d_context) return; g3d_context = g3d_context_new(); };
        static void DeleteG3DContext(void) { if(!g3d_context) return; g3d_context_free(g3d_context); g3d_context = 0; };
		#endif

		bool LoadFromFile(const char *file, int no_material = 0);
		bool LoadFromData(char *data, string path = "");
		bool LoadFromXML(rapidxml::xml_document<char> *doc, string path, int no_material);
        //int LoadFromFile_0_0(const char *file) { printf("LoadFromFile_0_0 function was removed.\n"); return 0; };
		//void SetOrientation(CVector o);
		//CVector ApplyOrientation(CVector v) { return Orientation(v, orient_x, orient_y, orient_z); };
		void PutToGL(tRenderer *renderer);
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

		tVertex *GetVertex(int i)					{ return vertices.at(i); }
		tTriangle *GetTriangle(int i)				{ return triangles.at(i); }
		tMeshMaterial *GetMaterial(int i)				{ return materials.at(i); }
		tMeshPose *GetCustomPose(string s) 			{ return custom_pose.at(s); }
		tAnimation *GetAnimation(int i)				{ return animations.at(i); }
		tEntity *GetEntity(int i)					{ return entities.at(i); }

		tMeshMaterial *GetIdleMaterial(void)			{ return idle_material; }

		map<string, tEntity *> GetEntitiesInGroup(const char *group = "");

		tBoundingBox GetBoundingBox(void)			{ return bounding_box; }

		void AddVertex(tVertex *v);
		void AddTriangle(tTriangle *t);
		void AddMaterial(tMeshMaterial *m);
		void AddCustomPose(string name, tMeshPose *p);
		void AddAnimation(tAnimation *a);

		void RemoveVertex(tVertex *v);
		void RemoveTriangle(tTriangle *t);
		void RemoveMaterial(tMeshMaterial *m);
		void RemoveCustomPose(string name);
		void RemoveAnimation(tAnimation *a);
		void RemoveEntity(tEntity *e);

		tIBO *CreateIBO(void)						{ return new tIBO(vao); }

		tMeshPose *GetIdlePose(void)		{ return idle_pose; }
		tMeshPose *GetCustomPoseByName(string name);
		tMeshPose *GetCurrentPose(void);
		string GetCurrentPoseName(string idle = string("Idle"));

		tAnimation *CreateAnimation(const char *name, float len = 1.0);
		void ChangeAnimation(tAnimation *a);
		void ChangeAnimation(const char *name);
		tAnimation *GetAnimationByName(const char *name);
		tAnimation *GetCurrentAnimation(void);
		char *GetCurrentAnimationName(void);
		void PlayAnimation(float t);
		void ApplyAnimation(void);
		void ResetAnimationFinished(void) { anim_finished = 0; };
		void SetAnimationLoop(int l) { loop_anim = l ? 1 : 0; anim_finished = 0; };
		int GetAnimationFinished(void) { return anim_finished; };

        void SetTriangleMaterials(void);
		void CalculateNormalsSolid(void);

		void GenerateBoundingBox(void);

		btTriangleMesh *GeneratePhysicsMesh(void);
		btTriangleMesh *GetPhysicsMesh(void)		{ return physics_triangle_mesh; }

		tVertex *CreateVertex(tVector v);
		tTriangle *CreateTriangle(tVertex *v1, tVertex *v2, tVertex *v3, tVector color, char material[100], tVector t1, tVector t2, tVector t3);
		tTriangle *CreateTriangleAuto(tVector v1, tVector v2, tVector v3, tVector color, char material[100], tVector t1, tVector t2, tVector t3);
		tMeshPose *CreateCustomPose(string name);
		tEntity *CreateEntity(string name, string group = string());

		tVBO<float> *CreateFloatVBO(int components);
		void AssignVertexArrayPositions(void);

		void ChangePose(string name, string idle = "Idle");
		void ChangePose(tMeshPose *pos);
		void CopyPoseFromVertices(void);

		tMeshMaterial *GetMaterialByName(string name);
		tVertex *GetVertexByID(int id);

		void SetVertexId(tVertex *v, int id);

		void TriggerAllVBOsRefresh(void)	{ refresh_vbos = true; }
		void TriggerIBOsRefresh(void)		{ refresh_ibos = true; }
		void RefreshIBOs(void);

		void SortTriangles(tVector cam);
		void SortMaterials(void);

		void SetIDs(void);

		void SetRefreshFunc(void (*func)(void)) { refresh_func = func; }

		tMesh(const char *file = 0);
		~tMesh(void);
};

#endif
















