
#ifndef _OBJECT_H
#define _OBJECT_H

class tObject
{
	protected:
		btRigidBody *rigid_body;
		btMotionState *motion_state;

	public:
		tObject(void);
		virtual ~tObject(void) {}

		virtual void GeometryPass(void) {};
		virtual void ForwardPass(void) {};
		virtual tBoundingBox GetBoundingBox(void) = 0;

		btRigidBody *GetRigidBody(void)		{ return rigid_body; }
};

class tTransformObject : public tObject
{
	protected:
		tTransform transform;

	public:
		tTransformObject(void)						{ this->transform = tTransform::GetIdentity(); }
		tTransformObject(tTransform transform)		{ this->transform = transform; }

		tTransform GetTransform(void)				{ return transform; }
		void SetTransform(tTransform transform);
		void SetTransformWithoutRigidBody(tTransform transform);
};


#endif
