
#ifndef _SCENE_H
#define _SCENE_H


class tScene
{
	private:
		tWorld *world;

		map<string, tAsset *> assets;
		map<string, tSceneObject *> objects;

		tCubeMapAsset *sky_cubemap;
		tSkyBox *skybox;

		void ParseSceneNode(xmlNodePtr cur);

		void ParseAssetsNode(xmlNodePtr cur);
		void ParseMeshAssetNode(xmlNodePtr cur);
		void ParseCubeMapAssetNode(xmlNodePtr cur);

		void ParseObjectsNode(xmlNodePtr cur);
		void ParseMeshObjectNode(xmlNodePtr cur);
		void ParseDirectionalLightObjectNode(xmlNodePtr cur);
		void ParsePointLightObjectNode(xmlNodePtr cur);

		static float ParseFloatNode(xmlNodePtr cur, const xmlChar *p = (const xmlChar *)"v");
		static tTransform ParseTransformNode(xmlNodePtr cur);
		static tVector ParseVectorNode(xmlNodePtr cur,	const xmlChar *x_p = (const xmlChar *)"x",
														const xmlChar *y_p = (const xmlChar *)"y",
														const xmlChar *z_p = (const xmlChar *)"z");

		void AddObject(std::string name, tSceneObject *object);

	public:
		tScene(tWorld *world);
		~tScene(void);

		bool LoadFromFile(string file);

		void AddToWorld(void);
		void RemoveFromWorld(void);
};



#endif
