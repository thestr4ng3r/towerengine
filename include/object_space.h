
#ifndef _TOWERENGINE_RENDERSPACE_H
#define _TOWERENGINE_RENDERSPACE_H

struct tObjectSpace
{
	std::unordered_set<tObject *> objects;

	virtual ~tObjectSpace(void)	{}

	virtual void AddObject(tObject *object);
	virtual void Clear(void);

	void DepthPrePass(tRenderer *renderer);
	void ShadowPass(tRenderer *renderer);
	void GeometryPass(tDeferredRenderer *renderer, bool cube_map_reflection_enabled = true);
	void ForwardPass(tDeferredRenderer *renderer);
	void RefractionPass(tDeferredRenderer *renderer);
	void StandardForwardPass(tForwardRenderer *renderer);
};

struct tRenderObjectSpace : public tObjectSpace
{
	std::set<tPointLight *> point_lights;
	std::set<tDirectionalLight *> dir_lights;

	virtual void AddObject(tObject *object);
	virtual void Clear(void);
};


#endif
