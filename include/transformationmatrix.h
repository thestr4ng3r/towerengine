
#ifndef _TRANSFORMATION_MATRIX_H
#define _TRANSFORMATION_MATRIX_H


class tTransformationMatrix
{
	private:
		float matrix[16];

	public:
		tTransformationMatrix(void)	{ LoadIdentity(); }

		float *GetMatrix(void)		{ return matrix; }

		void LoadIdentity(void);
		void Translate(tVector v);
		void RotateX(float a);
		void RotateY(float a);
		void RotateZ(float a);
		void SetXY(tVector x, tVector y);
		void SetYZ(tVector y, tVector z);
		void SetXZ(tVector x, tVector z);
		void SetXYZ(tVector x, tVector y, tVector z);
		void Rotate(tVector v);
		void Scale(tVector v);
};


#endif
