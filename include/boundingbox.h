
#ifndef _BOUNDINGBOX_H
#define _BOUNDINGBOX_H


class tBoundingBox
{
	private:
		tVector minv, maxv;

	public:
		tBoundingBox(void);
		tBoundingBox(tVector a, tVector b);

		tVector GetMin(void)	{ return minv; }
		tVector GetMax(void)	{ return maxv; }

		tVector *GetCornerPoints(void);

		void SetBounds(tVector a, tVector b);
		void AddPoint(tVector p);
};

tBoundingBox operator+(tBoundingBox a, const tVector &b);

#endif
