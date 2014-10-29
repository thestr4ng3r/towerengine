
#ifndef _SCENE_H
#define _SCENE_H


class tScene
{
	private:
		tWorld *world;

		map<string, tMesh *> meshes;
		map<string, tSceneObject *> objects;

		void ParseAssetsNode(xmlNodePtr cur);
		void ParseObjectsNode(xmlNodePtr cur);

	public:
		tScene(tWorld *world);
		~tScene(void);

		bool LoadFromFile(string file);

		void AddToWorld(void);
		void RemoveFromWorld(void);
};



#endif
