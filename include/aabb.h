
#ifndef _TOWERENGINE_AABB_H
#define _TOWERENGINE_AABB_H


class tAABB
{
	private:
		tVector minv, maxv;
		bool infinite;

	public:
		tAABB(void);
		tAABB(bool infinite);
		tAABB(tVector a, tVector b, bool infinite = false);

		static tAABB Infinite() 			{ return tAABB(true); }

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

tAABB operator+(tAABB a, const tVector &b);

#endif
