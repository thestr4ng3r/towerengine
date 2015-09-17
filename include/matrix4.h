
#ifndef _MATRIX4_H
#define _MATRIX4_H


class tMatrix4
{
	private:
		float v[16];

	public:
		static const float identity_matrix[16];

		tMatrix4(float v[16]);
		tMatrix4(void);

		void Set(float v[16]);

		float *GetData(void)	{ return v; }

		void SetIdentity(void);
		void SetTranslation(tVector t);
		void SetLookAt(tVector eye, tVector center, tVector up);
		void SetOrtho(float left, float right, float top, float bottom, float near_clip, float far_clip);
		void SetPerspective(float fovy, float aspect, float near_clip, float far_clip);
		void SetInverse(void);

		tMatrix4 GetInverse(void);

		void GetDataTranspose(float *out);

		void SetMultiply(tMatrix4 m);

		//void GLMultMatrix(void);

		static tMatrix4 GetIdentity(void)		{ return tMatrix4(); }
};

tMatrix4 operator*(tMatrix4 &a, tMatrix4 &b);

#endif
