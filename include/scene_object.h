

#ifndef _SCENE_OBJECT_H
#define _SCENE_OBJECT_H

#define T_SCENE_OBJECT_TYPE_OBJECT 1
#define T_SCENE_OBJECT_TYPE_DIRECTIONAL_LIGHT 2
#define T_SCENE_OBJECT_TYPE_POINT_LIGHT 3
#define T_SCENE_OBJECT_TYPE_EMPTY 4
#define T_SCENE_OBJECT_TYPE_CUBE_MAP_REFLECTION 5

class tSceneObject
{
	private:
		std::string tag;
		std::map<std::string, std::string> attributes;

	public:
		tSceneObject(void)			{ tag = ""; }
		virtual ~tSceneObject(void) {};

		virtual void AddToWorld(tWorld *world) =0;
		virtual void RemoveFromWorld(tWorld *world) =0;

		virtual int GetType(void) =0;

		void SetTag(std::string tag)		{ this->tag = tag; }
		std::string GetTag(void)			{ return tag; }

		void SetAttribute(std::string name, std::string value);
		std::string GetAttribute(std::string name);
		std::string GetAttribute(std::string name, std::string default_v);
		int GetAttributeInt(std::string name, int default_v = 0);
		float GetAttributeFloat(std::string name, float default_v = 0.0f);
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


class tEmptySceneObject : public tSceneObject
{
	private:
		tTransform transform;

	public:
		tEmptySceneObject(tTransform transform);
		~tEmptySceneObject(void);

		void AddToWorld(tWorld *world)		{}
		void RemoveFromWorld(tWorld *world)	{}

		int GetType(void)	{ return T_SCENE_OBJECT_TYPE_EMPTY; }

		tTransform GetTransform(void)		{ return transform; }
};


class tCubeMapReflectionSceneObject : public tSceneObject
{
	private:
		tCubeMapReflection *reflection;

	public:
		tCubeMapReflectionSceneObject(tCubeMapReflection *reflection);
		~tCubeMapReflectionSceneObject(void);

		void AddToWorld(tWorld *world);
		void RemoveFromWorld(tWorld *world);

		int GetType(void)	{ return T_SCENE_OBJECT_TYPE_CUBE_MAP_REFLECTION; }

		tCubeMapReflection *GetReflection(void)		{ return reflection; }
};


#endif
