
#include "towerengine.h"

#include <BulletCollision/CollisionShapes/btShapeHull.h>


tMeshObject::tMeshObject(tMesh *mesh, float mass)
{
	this->mesh = mesh;
	animation = 0;
	pose = cstr("Idle");
	animation_mode = 0;
	loop = false;
	pos = Vec(0.0, 0.0, 0.0);
	x = Vec(1.0, 0.0, 0.0);
	y = Vec(0.0, 1.0, 0.0);
	z = Vec(0.0, 0.0, 1.0);
	scale = Vec(1.0, 1.0, 1.0);
	transformation = new tTransformationMatrix();
	color = Vec(1.0, 1.0, 1.0);
	alpha = 1.0;
	visible = true;
	time = 0.0;
	motion_state = new tMeshObjectMotionState(this);
	if(mass > 0.0)
	{
		btVector3 inertia;
		//btBoxShape *shape = new btBoxShape(btVector3(1.0, 1.0, 1.0));

		btConvexTriangleMeshShape *shape = new btConvexTriangleMeshShape(mesh->GetPhysicsMesh());

		shape->calculateLocalInertia(mass, inertia);
		rigid_body = new btRigidBody(mass, motion_state, shape, inertia);
		//rigid_body->setActivationState(DISABLE_DEACTIVATION);
	}
	else
	{
		btCollisionShape *shape;
		if(mesh->GetPhysicsMesh())
			shape = new btBvhTriangleMeshShape(mesh->GetPhysicsMesh(), false);
		else
			shape = new btEmptyShape();

		rigid_body = new btRigidBody(0.0, motion_state, shape, btVector3(0.0, 0.0, 0.0));
	}
}

void tMeshObject::SetAnimation(const char *animation)
{
	tAnimation *a = mesh->GetAnimationByName(animation);
	if(a != this->animation)
		time = 0.0;
	this->animation = a;
}

void tMeshObject::Fade(float fade_end, float time)
{
	this->fade_end = fade_end;
	fade_speed = (fade_end - alpha) / time;
}

void tMeshObject::Play(float time)
{
	if(fade_speed != 0.0)
	{
		alpha += fade_speed * time;

		if((fade_speed > 0.0 && alpha >= fade_end)
				|| (fade_speed < 0.0 && alpha <= fade_end))
		{
			fade_speed = 0.0;
			alpha = fade_end;
		}
	}

	if(!animation_mode || !animation)
		return;

	if(!loop)
		this->time = min(animation->GetLength(), this->time + time);
	else
		this->time = fmod(this->time + time, animation->GetLength());
}

bool tMeshObject::GetAnimationFinished(void)
{
	if(!animation_mode || !animation)
		return true;

	return !loop && this->time >= animation->GetLength();
}

void tMeshObject::SetPose(const char *pose)
{
	if(this->pose)
		delete [] this->pose;
	this->pose = cstr(pose);
}

tBoundingBox tMeshObject::GetBoundingBox(void)
{
	tBoundingBox b;
	tVector *p = mesh->GetBoundingBox().GetCornerPoints();
	float *mat;

	transformation->LoadIdentity();
	transformation->Translate(pos);
	transformation->SetXYZ(x, y, z);
	transformation->Scale(scale);

	mat = transformation->GetMatrix();

	//btTransform trans(btMatrix3x3(mat[0], mat[1], mat[2], mat[4], mat[5], mat[6], mat[8], mat[9], mat[10]), btVector3(mat[3], mat[7], mat[11]));
	//rigid_body->setWorldTransform(trans);

	for(int i=0; i<8; i++)
		b.AddPoint(ApplyMatrix4(mat, p[i]));

	return b;
}

void tMeshObject::GeometryPass(void)
{
	if(!visible || alpha <= 0.0)
		return;

	transformation->LoadIdentity();
	transformation->Translate(pos);
	transformation->SetXYZ(x, y, z);
	transformation->Scale(scale);

	tEngine::GetCurrentFaceShader()->SetTransformation(transformation->GetMatrix());

	tMesh::Color(color, alpha);

	if(animation_mode && animation)
	{
		mesh->ChangeAnimation(animation);
		mesh->SetAnimationLoop(0);
		animation->SetTime(time);
	}
	else
	{
		if(pose)
			mesh->ChangePose(pose);
		else
			mesh->ChangePose("Idle");
	}
	mesh->PutToGL();
}

void tMeshObject::ForwardPass(void)
{

}

void tMeshObject::UpdateRigidBodyTransformation(void)
{
	btTransform trans;

	motion_state->getWorldTransform(trans);
	rigid_body->setWorldTransform(trans);
	rigid_body->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
	rigid_body->setAngularVelocity(btVector3(0.0, 0.0, 0.0));
	rigid_body->activate();
}


tMeshObjectMotionState::tMeshObjectMotionState(tMeshObject *object)
{
	this->object = object;
}

void tMeshObjectMotionState::getWorldTransform(btTransform &trans) const
{
	trans.setOrigin(BtVec(object->GetPosition()));
	tVector x = object->GetX();
	tVector y = object->GetY();
	tVector z = object->GetZ();
	trans.setBasis(btMatrix3x3(x.x, x.y, x.z, y.x, y.y, y.z, z.x, z.y, z.z));
}

void tMeshObjectMotionState::setWorldTransform(const btTransform &trans)
{
	object->SetPosition(Vec(trans.getOrigin()));
	tVector x, y, z;
	x = trans.getBasis().getRow(0);
	y = trans.getBasis().getRow(1);
	z = trans.getBasis().getRow(2);
	object->SetXYZ(x, y, z);
}




