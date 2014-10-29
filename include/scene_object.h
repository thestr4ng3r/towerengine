

#ifndef _SCENE_OBJECT_H
#define _SCENE_OBJECT_H

#define SCENE_OBJECT_TYPE_OBJECT 1
#define SCENE_OBJECT_TYPE_DIRECTIONAL_LIGHT 2
#define SCENE_OBJECT_TYPE_POINT_LIGHT 3

class CSceneObject
{
	public:
		virtual ~CSceneObject(void) {};

		virtual void AddToWorld(CWorld *world) =0;
		virtual void RemoveFromWorld(CWorld *world) =0;

		virtual int GetType(void) =0;
};

class CObjectSceneObject : public CSceneObject
{
	private:
		CObject *object;

	public:
		CObjectSceneObject(CObject *object);
		~CObjectSceneObject(void);

		void AddToWorld(CWorld *world);
		void RemoveFromWorld(CWorld *world);

		int GetType(void)	{ return SCENE_OBJECT_TYPE_OBJECT; }

};


#endif
