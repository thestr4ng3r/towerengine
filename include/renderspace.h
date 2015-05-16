
#ifndef _RENDERSPACE_H
#define _RENDERSPACE_H


struct tRenderObjectSpace
{
	set<tObject *> objects;

	virtual ~tRenderObjectSpace(void)	{}

	virtual void Clear(void);

	void GeometryPass(tRenderer *renderer);
	void ForwardPass(tRenderer *renderer);
};

struct tRenderSpace : public tRenderObjectSpace
{
	set<tPointLight *> point_lights;
	set<tDirectionalLight *> dir_lights;

	void Clear(void);
};


#endif
