

#ifndef _UTILOBJECTS_H
#define _UTILOBJECTS_H


class CCoordinateSystemObject : public CObject
{
	private:
		CVector pos;
		CVector size;
		bool depth_test;

	public:
		CCoordinateSystemObject(bool depth_test = false);
		~CCoordinateSystemObject(void);

		void SetPosition(CVector pos)	{ this->pos = pos; }
		void SetSize(CVector size)		{ this->size = size; }

		void ForwardPass(void);

		CBoundingBox GetBoundingBox(void);
};


#endif
