

#ifndef _SCENE_OBJECT_H
#define _SCENE_OBJECT_H

#define T_SCENE_OBJECT_TYPE_OBJECT 1
#define T_SCENE_OBJECT_TYPE_DIRECTIONAL_LIGHT 2
#define T_SCENE_OBJECT_TYPE_POINT_LIGHT 3

class tSceneObject
{
	public:
		virtual ~tSceneObject(void) {};

		virtual void AddToWorld(tWorld *world) =0;
		virtual void RemoveFromWorld(tWorld *world) =0;

		virtual int GetType(void) =0;
};

class tObjectSceneObject : public tSceneObject
{
	private:
		tObject *object;

	public:
		tObjectSceneObject(tObject *object);
		~tObjectSceneObject(void);

		void AddToWorld(tWorld *world);
		void RemoveFromWorld(tWorld *world);

		int GetType(void)	{ return T_SCENE_OBJECT_TYPE_OBJECT; }

};


#endif
