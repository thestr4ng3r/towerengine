
#ifndef _SCENE_H
#define _SCENE_H


class CScene
{
	private:
		CWorld *world;

		map<string, CMesh *> meshes;
		map<string, CSceneObject *> objects;

		void ParseAssetsNode(xmlNodePtr cur);
		void ParseObjectsNode(xmlNodePtr cur);

	public:
		CScene(CWorld *world);
		~CScene(void);

		bool LoadFromFile(string file);

		void AddToWorld(void);
		void RemoveFromWorld(void);
};



#endif
