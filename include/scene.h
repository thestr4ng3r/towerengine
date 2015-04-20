
#ifndef _SCENE_H
#define _SCENE_H


// rapidxml forward declarations
namespace rapidxml
{
    template<class Ch> class xml_node;
    template<class Ch> class xml_attribute;
    template<class Ch> class xml_document;
}


class tScene
{
	private:
		tWorld *world;

		string load_path;

		map<string, tAsset *> assets;
		map<string, tSceneObject *> objects;

		tCubeMapAsset *sky_cubemap;
		tSkyBox *skybox;

		void ParseSceneNode(rapidxml::xml_node<char> *cur);

		void ParseAssetsNode(rapidxml::xml_node<char> *cur);
		void ParseMeshAssetNode(rapidxml::xml_node<char> *cur);
		void ParseCubeMapAssetNode(rapidxml::xml_node<char> *cur);

		void ParseObjectsNode(rapidxml::xml_node<char> *cur);
		void ParseMeshObjectNode(rapidxml::xml_node<char> *cur);
		void ParseDirectionalLightObjectNode(rapidxml::xml_node<char> *cur);
		void ParsePointLightObjectNode(rapidxml::xml_node<char> *cur);

		static float ParseFloatNode(rapidxml::xml_node<char> *cur, const char *p = "v");
		static tTransform ParseTransformNode(rapidxml::xml_node<char> *cur);
		static tVector ParseVectorNode(rapidxml::xml_node<char> *cur,	const char *x_p = "x",
																		const char *y_p = "y",
																		const char *z_p = "z");

		void AddObject(std::string name, tSceneObject *object);

	public:
		tScene(tWorld *world);
		~tScene(void);

		bool LoadFromFile(string file);

		void AddToWorld(void);
		void RemoveFromWorld(void);

		tSceneObject *GetObjectByTag(string tag);
		list<tSceneObject *> *GetObjectsByTag(string tag);
		tSceneObject *GetObjectWhereTagStartsWith(string start);
		list<tSceneObject *> *GetObjectsWhereTagStartsWith(string start);
};



#endif
