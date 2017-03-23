
#ifndef _RENDERSPACE_H
#define _RENDERSPACE_H


struct tRenderObjectSpace
{
	std::set<tObject *> objects;

	virtual ~tRenderObjectSpace(void)	{}

	virtual void Clear(void);

	void DepthPrePass(tRenderer *renderer);
	void ShadowPass(tRenderer *renderer);
	void GeometryPass(tDeferredRenderer *renderer, bool cube_map_reflection_enabled = true);
	void ForwardPass(tDeferredRenderer *renderer);
	void RefractionPass(tDeferredRenderer *renderer);
};

struct tRenderSpace : public tRenderObjectSpace
{
	std::set<tPointLight *> point_lights;
	std::set<tDirectionalLight *> dir_lights;

	void Clear(void);
};


#endif
