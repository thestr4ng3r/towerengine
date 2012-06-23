
#ifndef _WORLD_H
#define _WORLD_H

class CWorld
{
private:
	vector<CObject *> objects;

public:
	CWorld(void);
	~CWorld(void);

	void AddObject(CObject *o);
	void RemoveObject(CObject *o);

	void PutToGL(CVector cam_pos);
};


#endif
