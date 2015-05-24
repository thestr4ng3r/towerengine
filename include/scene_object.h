

#ifndef _SCENE_OBJECT_H
#define _SCENE_OBJECT_H

#define T_SCENE_OBJECT_TYPE_OBJECT 1
#define T_SCENE_OBJECT_TYPE_DIRECTIONAL_LIGHT 2
#define T_SCENE_OBJECT_TYPE_POINT_LIGHT 3

class tSceneObject
{
	private:
		std::string tag;

	public:
		tSceneObject(void)			{ tag = ""; }
		virtual ~tSceneObject(void) {};

		virtual void AddToWorld(tWorld *world) =0;
		virtual void RemoveFromWorld(tWorld *world) =0;

		virtual int GetType(void) =0;

		void SetTag(std::string tag)		{ this->tag = tag; }
		std::string GetTag(void)			{ return tag; }
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

		tObject *GetObject(void)	{ return object; }

};

class tDirectionalLightSceneObject : public tSceneObject
{
	private:
		tDirectionalLight *light;

	public:
		tDirectionalLightSceneObject(tDirectionalLight *light);
		~tDirectionalLightSceneObject(void);

		void AddToWorld(tWorld *world);
		void RemoveFromWorld(tWorld *world);

		int GetType(void)	{ return T_SCENE_OBJECT_TYPE_DIRECTIONAL_LIGHT; }

		tDirectionalLight *GetLight(void)	{ return light; }
};

class tPointLightSceneObject : public tSceneObject
{
	private:
		tPointLight *light;

	public:
		tPointLightSceneObject(tPointLight *light);
		~tPointLightSceneObject(void);

		void AddToWorld(tWorld *world);
		void RemoveFromWorld(tWorld *world);

		int GetType(void)	{ return T_SCENE_OBJECT_TYPE_POINT_LIGHT; }

		tPointLight *GetLight(void)		{ return light; }
};


#endif
