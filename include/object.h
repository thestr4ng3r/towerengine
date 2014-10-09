
#ifndef _OBJECT_H
#define _OBJECT_H

class CObject
{
	protected:
		btRigidBody *rigid_body;
		btMotionState *motion_state;

	public:
		CObject(void);
		virtual ~CObject(void) {}

		virtual void GeometryPass(void) {};
		virtual void ForwardPass(void) {};
		virtual CBoundingBox GetBoundingBox(void) = 0;

		btRigidBody *GetRigidBody(void)		{ return rigid_body; }
};


#endif
