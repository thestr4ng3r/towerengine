
#ifndef _CHARACTERCONTROLLER_H
#define _CHARACTERCONTROLLER_H


class CCharacterController : public btActionInterface
{
	private:
		CVector velocity;
		float vertical_velocity;

		float step_height;

		bool on_ground;

		btRigidBody *body;
		btConvexShape *shape;

	public:
		CCharacterController(btRigidBody *body, btConvexShape *shape, float step_height);

		virtual void updateAction(btCollisionWorld *world, btScalar time);

		void debugDraw(btIDebugDraw* debugDrawer) {};


		void SetVelocity(CVector v)		{ velocity = v; }
		void Jump(float v)				{ if(on_ground) vertical_velocity += v; }

		bool GetOnGround(void)			{ return on_ground; }


};


#endif
