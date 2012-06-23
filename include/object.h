
#ifndef _OBJECT_H
#define _OBJECT_H

class CObject
{
public:
	virtual ~CObject(void) {}

	virtual void PutToGL(CVector cam = Vec(0.0, 0.0, 0.0)) = 0;
};


#endif
