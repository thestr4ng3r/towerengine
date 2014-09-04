
#ifndef _OBJECT_H
#define _OBJECT_H

class CObject
{
	protected:
		btRigidBody *rigid_body;
		btDefaultMotionState *motion_state;

	public:
		virtual ~CObject(void) {}

		virtual void PutToGL(void) = 0;
		virtual CBoundingBox GetBoundingBox(void) = 0;

		btRigidBody *GetRigidBody(void)		{ return rigid_body; }
};


#endif
