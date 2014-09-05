
#include "towerengine.h"



class CCharacterControllerCollisionCallback : public btCollisionWorld::ClosestConvexResultCallback
{
	public:
		CCharacterControllerCollisionCallback (btCollisionObject* me)
				: btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
		{
			this->me = me;
		}

		virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult &convexResult, bool normalInWorldSpace)
		{
			if(convexResult.m_hitCollisionObject == me)
				return btScalar(1.0);

			if(!convexResult.m_hitCollisionObject->hasContactResponse())
				return btScalar(1.0);

			btVector3 hitNormalWorld;
			if (normalInWorldSpace)
			{
				hitNormalWorld = convexResult.m_hitNormalLocal;
			}
			else
			{
				///need to transform normal into worldspace
				hitNormalWorld = convexResult.m_hitCollisionObject->getWorldTransform().getBasis()*convexResult.m_hitNormalLocal;
			}

			return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
		}
	protected:
		btCollisionObject* me;
};




CCharacterController::CCharacterController(btRigidBody *body, btConvexShape *shape, float step_height)
{
	this->body = body;
	this->shape = shape;
	velocity = Vec(0.0, 0.0, 0.0);
	vertical_velocity = 0.0;
	on_ground = false;

	this->step_height = step_height;

	body->setAngularFactor(0.0);
	body->setLinearFactor(btVector3(1.0, 0.0, 1.0));
}



void CCharacterController::updateAction(btCollisionWorld *world, btScalar time)
{
	btVector3 pos;

	btTransform start = body->getWorldTransform();
	btTransform end = body->getWorldTransform();

	btVector3 start_pos;
	btVector3 target_pos;

	body->setLinearVelocity(btVector3(0.0, 0.0, 0.0));


	vertical_velocity -= 9.81 * time;
	end.setOrigin(start.getOrigin() + btVector3(0.0, vertical_velocity * time, 0.0));

	CCharacterControllerCollisionCallback callback_vert = CCharacterControllerCollisionCallback(body);

	world->convexSweepTest(shape, start, end, callback_vert, 0.005);

	on_ground = false;

	if(callback_vert.hasHit())
	{
		pos.setInterpolate3(start.getOrigin(), end.getOrigin(), callback_vert.m_closestHitFraction);
		start.setOrigin(pos);

		float rest_v_velocity = (1.0 - callback_vert.m_closestHitFraction) * vertical_velocity * time;

		if(callback_vert.m_hitNormalWorld.dot(btVector3(0.0, 1.0, 0.0)) > 0.7071 || vertical_velocity > 0.0)
		{
			if(vertical_velocity < 0.0)
				on_ground = true;
			vertical_velocity = 0.0;
			end = start;
		}
		else
		{
			btVector3 slide_vec = btVector3(0.0, 1.0, 0.0).cross(callback_vert.m_hitNormalWorld);
			slide_vec = slide_vec.cross(callback_vert.m_hitNormalWorld);
			slide_vec.normalize();
			slide_vec *= slide_vec.dot(btVector3(0.0, -1.0, 0.0)) * (-rest_v_velocity);

			end.setOrigin(pos + slide_vec);
			CCharacterControllerCollisionCallback callback_vert_slide = CCharacterControllerCollisionCallback(body);
			world->convexSweepTest(shape, start, end, callback_vert_slide, 0.005);

			if(callback_vert_slide.hasHit())
			{
				pos.setInterpolate3(start.getOrigin(), end.getOrigin(), callback_vert_slide.m_closestHitFraction);
				end.setOrigin(pos);
			}
			start = end;
		}
	}

	body->setWorldTransform(end);


	start_pos = start.getOrigin();
	target_pos = start.getOrigin() + BtVec(velocity * time);

	end.setOrigin(target_pos);

	CCharacterControllerCollisionCallback callback = CCharacterControllerCollisionCallback(body);

	world->convexSweepTest(shape, start, end, callback);

	if(!callback.hasHit()) // no hits
	{
		//body->setWorldTransform(end);
		body->setLinearVelocity(BtVec(velocity * time) * 60.0);
		return;
	}


	btVector3 first_collision_pos;
	btVector3 rest_walk_dir;
	float real_step = step_height;
	btScalar forward_step;


	first_collision_pos.setInterpolate3(start.getOrigin(), end.getOrigin(), callback.m_closestHitFraction);
	pos = first_collision_pos;

	rest_walk_dir = BtVec(velocity * time) * (1.0 - callback.m_closestHitFraction);


	// trying to climb...

	start.setOrigin(pos);
	end.setOrigin(pos + btVector3(0.0, step_height, 0.0));

	// go up

	CCharacterControllerCollisionCallback callback_up = CCharacterControllerCollisionCallback(body);
	world->convexSweepTest(shape, start, end, callback_up, 0.001);

	if(callback_up.hasHit())
	{
		pos.setInterpolate3(start.getOrigin(), end.getOrigin(), callback_up.m_closestHitFraction);
		pos.setY(pos.y());
		real_step = callback_up.m_closestHitFraction * step_height;
	}
	else
		pos = end.getOrigin();


	start.setOrigin(pos);
	end.setOrigin(pos + rest_walk_dir);

	// go forward

	CCharacterControllerCollisionCallback callback_f = CCharacterControllerCollisionCallback(body);
	world->convexSweepTest(shape, start, end, callback_f);

	forward_step = rest_walk_dir.length();

	if(callback_f.hasHit())
	{
		pos.setInterpolate3(start.getOrigin(), end.getOrigin(), callback_f.m_closestHitFraction);
		forward_step *= callback_f.m_closestHitFraction;
	}
	else
		pos = end.getOrigin();


	start.setOrigin(pos);
	end.setOrigin(pos - btVector3(0.0, real_step, 0.0));


	if(forward_step < 0.0001)
	{
		body->setLinearVelocity(BtVec(velocity * time) * 60.0);
		return;
	}

	// go down again

	CCharacterControllerCollisionCallback callback_down = CCharacterControllerCollisionCallback(body);
	world->convexSweepTest(shape, start, end, callback_down);

	if(callback_down.hasHit())
	{
		if(callback_down.m_closestHitFraction * real_step < forward_step) // too steep
		{
			//pos = target_pos + callback.m_hitNormalWorld * callback.m_hitNormalWorld.dot(first_collision_pos - target_pos);
			body->setLinearVelocity(BtVec(velocity * time) * 60.0);
		}
		else
		{
			pos.setInterpolate3(start.getOrigin(), end.getOrigin(), callback_down.m_closestHitFraction);
			end.setOrigin(pos);
			body->setWorldTransform(end);
			//body->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
		}
	}
	else
	{
		//pos = end.getOrigin();
		body->setWorldTransform(end);
		//body->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
	}
}


