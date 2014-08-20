
#ifndef _BOUNDINGBOX_H
#define _BOUNDINGBOX_H


class CBoundingBox
{
	private:
		CVector minv, maxv;

	public:
		CBoundingBox(void);
		CBoundingBox(CVector a, CVector b);

		CVector GetMin(void)	{ return minv; }
		CVector GetMax(void)	{ return maxv; }

		CVector *GetCornerPoints(void);

		void SetBounds(CVector a, CVector b);
		void AddPoint(CVector p);
};

CBoundingBox operator+(CBoundingBox a, const CVector &b);

#endif
