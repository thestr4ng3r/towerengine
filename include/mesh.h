#ifndef MESH_H
#define MESH_H

#define TEM_VERSION_0_1				1
#define TEM_VERSION_0_2				2
#define TEM_VERSION_0_3				3
#define TEM_VERSION_0_4				3

#define TEM_VERSION_0_1_STRING		"0.1"
#define TEM_VERSION_0_2_STRING		"0.2"
#define TEM_VERSION_0_3_STRING		"0.3"
#define TEM_VERSION_0_4_STRING		"0.4"

#define TEM_CURRENT_VERSION			TEM_VERSION_0_4
#define TEM_CURRENT_VERSION_STRING	TEM_VERSION_0_4_STRING

template <class T> class tVBO;

// rapidxml forward declarations
namespace rapidxml
{
    template<class Ch> class xml_node;
    template<class Ch> class xml_attribute;
    template<class Ch> class xml_document;
}

typedef unsigned short tVertexIndex;

struct tVertex
{
	tVector pos;
	tVector2 uv;
	tVector normal;
	tVector tang;
	tVector bitang;
};

struct tTriangle
{
	tVertexIndex v[3];
	tMaterial *mat;
};

class tMesh
{
	private:
		bool loop_anim;
		bool anim_finished;
		bool animation_mode;

		bool refresh_vbos;
		bool refresh_ibos;

		tVAO *vao;
		tVBO<float> *vertex_vbo;
		tVBO<float> *normal_vbo;
		tVBO<float> *tang_vbo;
		tVBO<float> *bitang_vbo;
		tVBO<float> *face_normal_vbo;
		tVBO<float> *uvcoord_vbo;

		std::map<tMaterial *, tMaterialIBO *> material_ibos;

		tBoundingBox bounding_box;

		std::set<tVertex *> outdated_vertices;

		tMeshPose *current_pose;
		tAnimation *current_animation;

		tMeshPose *idle_pose;

		tMaterial *idle_material;

		std::vector<tVertex> vertices;
		std::vector<tTriangle> triangles;
		std::map<std::string, tMaterial *> materials;
		std::map<std::string, tMeshPose *> custom_pose;
		std::vector<tAnimation *> animations;
		std::vector<tEntity *> entities;

		std::set<tMaterial *> own_materials;

		std::map<int, tVertex *> vertex_indices;


		btTriangleMesh *physics_triangle_mesh;


		void RefreshAllVBOs(void);
		void DeleteVBOData(void);

		tVertexIndex ParseVertexNode(rapidxml::xml_node<char> *cur);
		tMaterial *ParseMaterialNode(rapidxml::xml_node<char> *cur, std::string path);
		void ParseTriangleNode(rapidxml::xml_node<char> *cur, tMaterialManager *material_manager);
		void ParseMeshDataNode(rapidxml::xml_node<char> *cur, tMaterialManager *material_manager, int &current_vertex_id);
		tMeshPose *ParsePoseNode(rapidxml::xml_node<char> *cur);
		tAnimation *ParseAnimationNode(rapidxml::xml_node<char> *cur);
		tKeyFrame *ParseKeyFrameNode(rapidxml::xml_node<char> *cur, tAnimation *anim);
		tEntity *ParseEntityNode(rapidxml::xml_node<char> *cur);


		static void LoadTextureFromXMLNodeData(rapidxml::xml_node<char> *node, tDefaultMaterial *material, tDefaultMaterial::TextureType texture_type);
		static tDefaultMaterial *ParseXMLDefaultMaterialNode(rapidxml::xml_node<char> *cur, std::string &name, std::string path);
		static tSimpleForwardMaterial *ParseXMLSimpleForwardMaterialNode(rapidxml::xml_node<char> *cur, std::string &name, std::string path);
		static tRefractionMaterial *ParseXMLRefractionMaterialNode(rapidxml::xml_node<char> *cur, std::string &name, std::string path);

		void CalculateAllTangents();
		void CalculateTangents(const tTriangle &t);

	public:
		static tMaterial *ParseXMLMaterialNode(rapidxml::xml_node<char> *cur, std::string &name, std::string path);

		bool LoadFromFile(const char *file, tMaterialManager *material_manager = 0);
		bool LoadFromData(char *data, std::string path = "", tMaterialManager *material_manager = 0);
		bool LoadFromXML(rapidxml::xml_document<char> *doc, std::string path, tMaterialManager *material_manager = 0);

		void DepthPrePass(tRenderer *renderer, std::map<tMaterial *, tMaterial *> *replace_materials);
		void GeometryPass(tRenderer *renderer, std::map<tMaterial *, tMaterial *> *replace_materials);
		void ForwardPass(tRenderer *renderer, float *transform, std::map<tMaterial *, tMaterial *> *replace_materials);
		void RefractionPass(tRenderer *renderer, float *transform, std::map<tMaterial *, tMaterial *> *replace_materials);

		bool GetCubeMapReflectionEnabled(void);

		int GetVertexCount(void)					{ return vertices.size(); }
		int GetTriangleCount(void)					{ return triangles.size(); }
		int GetMaterialCount(void)					{ return materials.size(); }
		int GetCustomPosesCount(void)				{ return custom_pose.size(); }
		int GetAnimationCount(void)					{ return animations.size(); }
		int GetEntityCount(void)					{ return entities.size(); }

		tVertex &GetVertex(tVertexIndex i)			{ return vertices[i]; }
		tTriangle &GetTriangle(int i)				{ return triangles[i]; }
		tMeshPose *GetCustomPose(std::string s) 			{ return custom_pose.at(s); }
		tAnimation *GetAnimation(int i)				{ return animations.at(i); }
		tEntity *GetEntity(int i)					{ return entities.at(i); }

		tMaterial *GetIdleMaterial(void)			{ return idle_material; }

		std::map<std::string, tEntity *> GetEntitiesInGroup(const char *group = "");

		tBoundingBox GetBoundingBox(void)			{ return bounding_box; }

		tVertexIndex AddVertex(tVertex v);
		void AddTriangle(tTriangle t);
		void AddMaterial(std::string name, tMaterial *m);
		void AddCustomPose(std::string name, tMeshPose *p);
		void AddAnimation(tAnimation *a);

		/*void RemoveVertex(tVertex *v);
		void RemoveTriangle(tTriangle *t);
		void RemoveMaterial(std::string name);
		void RemoveCustomPose(std::string name);
		void RemoveAnimation(tAnimation *a);
		void RemoveEntity(tEntity *e);*/

		tMeshPose *GetIdlePose(void)		{ return idle_pose; }
		tMeshPose *GetCustomPoseByName(std::string name);
		tMeshPose *GetCurrentPose(void);
		std::string GetCurrentPoseName(std::string idle = std::string("Idle"));

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

		void GenerateBoundingBox(void);

		btTriangleMesh *GeneratePhysicsMesh(void);
		btTriangleMesh *GetPhysicsMesh(void)		{ return physics_triangle_mesh; }

		tMeshPose *CreateCustomPose(std::string name);
		tEntity *CreateEntity(std::string name, std::string group = std::string());

		void ChangePose(std::string name, std::string idle = "Idle");
		void ChangePose(tMeshPose *pos);
		void CopyPoseFromVertices(void);

		tMaterial *GetMaterialByName(std::string name);

		void TriggerAllVBOsRefresh(void)	{ refresh_vbos = true; }
		void TriggerIBOsRefresh(void)		{ refresh_ibos = true; }
		void RefreshIBOs(void);

		void SortMaterials(void);

		tMesh(const char *file = 0, tMaterialManager *material_manager = 0);
		~tMesh(void);
};

#endif
















