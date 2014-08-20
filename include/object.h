
#ifndef _OBJECT_H
#define _OBJECT_H

class CObject
{
public:
	virtual ~CObject(void) {}

	virtual void PutToGL(void) = 0;
	virtual CBoundingBox GetBoundingBox(void) = 0;
};


#endif
