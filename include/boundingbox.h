
#ifndef _BOUNDINGBOX_H
#define _BOUNDINGBOX_H


class tBoundingBox
{
	private:
		tVector minv, maxv;
		bool infinite;

	public:
		tBoundingBox(void);
		tBoundingBox(bool infinite);
		tBoundingBox(tVector a, tVector b, bool infinite = false);

		static tBoundingBox Infinite() 			{ return tBoundingBox(true); }

		const tVector &GetMin(void) const		{ return minv; }
		const tVector &GetMax(void) const		{ return maxv; }
		const bool &GetInfinite(void) const		{ return infinite; }

		void GetCornerPoints(tVector p[8]);

		void SetBounds(tVector a, tVector b);
		void SetInfinite(bool infinite)	{ this->infinite = infinite; }
		void AddPoint(tVector p);

		bool ContainsPoint(tVector p);
		tVector GetNormalizedDistanceToCenter(tVector pos);
		float GetNormalizedBoxDistanceToCenter(tVector pos);
};

tBoundingBox operator+(tBoundingBox a, const tVector &b);

#endif
