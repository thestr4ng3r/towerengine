
#ifndef _CULLING_H
#define _CULLING_H


class tCulling
{
	public:
		virtual ~tCulling(void) {}

		virtual void InitCulling(void) {}

		virtual bool TestPointCulling(tVector point) =0;
		virtual bool TestSphereCulling(tVector center, float radius) =0;
		virtual bool TestBoundingBoxCulling(tBoundingBox b) =0;
};



#endif
