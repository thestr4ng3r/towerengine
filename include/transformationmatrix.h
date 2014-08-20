
#ifndef _CTRANSFORMATION_H
#define _CTRANSFORMATION_H


class CTransformationMatrix
{
	private:
		float matrix[16];

	public:
		CTransformationMatrix(void)	{ LoadIdentity(); }

		float *GetMatrix(void)		{ return matrix; }

		void LoadIdentity(void);
		void Translate(CVector v);
		void RotateX(float a);
		void RotateY(float a);
		void RotateZ(float a);
		void SetXY(CVector x, CVector y);
		void SetYZ(CVector y, CVector z);
		void SetXZ(CVector x, CVector z);
		void SetXYZ(CVector x, CVector y, CVector z);
		void Rotate(CVector v);
		void Scale(CVector v);
};


#endif
