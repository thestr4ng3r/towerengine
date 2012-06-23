
#ifndef _CAMERA_H
#define _CAMERA_H

class CCamera
{
public:
	virtual ~CCamera(void) {};

	virtual CVector GetPosition(void) = 0;
	virtual void SetGLMatrix(void) = 0;
};

class CDirCamera : public CCamera
{
private:
	CVector pos;
	CVector dir;
	CVector top;

public:
	CDirCamera(void);

	CVector GetPosition(void)		{ return pos; }

	void SetPosition(CVector p)	{ pos = p; }
	void SetDirection(CVector d)	{ dir = d; dir.Normalize(); }
	void SetTop(CVector t)			{ top = t; top.Normalize(); }

	void SetGLMatrix(void);
};

#endif
