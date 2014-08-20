
#ifndef _RENDERSPACE_H
#define _RENDERSPACE_H


class CRenderSpace
{
	public:
		set<CObject *> objects;

	public:
		void ClearObjects(void);

		void Render(void);
};


#endif
