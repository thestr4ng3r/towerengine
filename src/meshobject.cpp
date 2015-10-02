
#include "towerengine.h"

#include <BulletCollision/CollisionShapes/btShapeHull.h>

using namespace std;

tMeshObject::tMeshObject(tMesh *mesh) : tTransformObject()
{
	this->mesh = mesh;
	rigid_body = 0;
	motion_state = 0;
	animation = 0;
	pose = cstr("Idle");
	animation_mode = 0;
	loop = false;
	color = Vec(1.0, 1.0, 1.0);
	alpha = 1.0;
	visible = true;
	time = 0.0;
	transform_matrix = new float[16];
	motion_state = new tMeshObjectMotionState(this);

	rigid_body = 0;
}

void tMeshObject::TransformChanged(void)
{
	if(rigid_body)
		rigid_body->setWorldTransform(transform.ToBtTransform());
}

void tMeshObject::SetTransformWithoutRigidBody(tTransform transform)
{
	this->transform = transform;
}


void tMeshObject::AddedToWorld(tWorld *world)
{
	if(rigid_body)
		world->GetDynamicsWorld()->addRigidBody(rigid_body);
}

void tMeshObject::RemovedFromWorld(tWorld *world)
{
	if(rigid_body)
		world->GetDynamicsWorld()->removeRigidBody(rigid_body);
}


void tMeshObject::SetAnimation(const char *animation)
{
	tAnimation *a = mesh->GetAnimationByName(animation);
	if(a != this->animation)
		time = 0.0;
	this->animation = a;
}

void tMeshObject::Play(float time)
{
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

	//btTransform trans(btMatrix3x3(mat[0], mat[1], mat[2], mat[4], mat[5], mat[6], mat[8], mat[9], mat[10]), btVector3(mat[3], mat[7], mat[11]));
	//rigid_body->setWorldTransform(trans);

	for(int i=0; i<8; i++)
		b.AddPoint(p[i] * transform);

	return b;
}

void tMeshObject::DepthPrePass(tRenderer *renderer)
{
	if(!visible || alpha <= 0.0 || !mesh)
		return;

	float *temp = transform.GetMatrix(transform_matrix);

	renderer->GetCurrentFaceShader()->SetTransformation(temp);

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
	mesh->DepthPrePass(renderer);
}

void tMeshObject::GeometryPass(tRenderer *renderer)
{
	if(!visible || alpha <= 0.0 || !mesh)
		return;

	float *temp = transform.GetMatrix(transform_matrix);

	renderer->GetCurrentFaceShader()->SetTransformation(temp);

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
	mesh->GeometryPass(renderer);
}

void tMeshObject::ForwardPass(tRenderer *renderer)
{
	if(!visible || alpha <= 0.0 || !mesh)
		return;

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

	float *temp = transform.GetMatrix(transform_matrix);
	mesh->ForwardPass(renderer, temp);
}


void tMeshObject::RefractionPass(tRenderer *renderer)
{
	if(!visible || alpha <= 0.0 || !mesh)
		return;

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

	float *temp = transform.GetMatrix(transform_matrix);
	mesh->RefractionPass(renderer, temp);
}


void tMeshObject::InitMeshRigidBody(float mass)
{
	DeleteRigidBody();

	if(mass > 0.0)
	{
		btVector3 inertia;
		//btBoxShape *shape = new btBoxShape(btVector3(0.2, 0.2, 0.2));

		btCollisionShape *shape;
		if(mesh->GetPhysicsMesh())
			shape = new btConvexTriangleMeshShape(mesh->GetPhysicsMesh());
		else
			shape = new btEmptyShape();

		shape->setMargin(0.01);
		shape->calculateLocalInertia(mass, inertia);

		rigid_body = new btRigidBody(mass, motion_state, shape, inertia);
		//rigid_body->setActivationState(DISABLE_DEACTIVATION);
	}
	else if(mass == 0.0)
	{
		btCollisionShape *shape;
		if(mesh->GetPhysicsMesh())
			shape = new btBvhTriangleMeshShape(mesh->GetPhysicsMesh(), true);
		else
			shape = new btEmptyShape();

		shape->setMargin(0.01);

		rigid_body = new btRigidBody(0.0, motion_state, shape, btVector3(0.0, 0.0, 0.0));
	}

	if(rigid_body)
	{
		rigid_body->setUserPointer(this);

		tWorld *world = GetWorld();
		if(world)
			world->GetDynamicsWorld()->addRigidBody(rigid_body);
	}
}

void tMeshObject::DeleteRigidBody(void)
{
	if(!rigid_body)
		return;

	tWorld *world = GetWorld();
	if(world)
		world->GetDynamicsWorld()->removeRigidBody(rigid_body);

	delete rigid_body;
	rigid_body = 0;
}

void tMeshObject::UpdateRigidBodyTransformation(void)
{
	if(!rigid_body)
		return;

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
	trans = object->GetTransform().ToBtTransform();
}

void tMeshObjectMotionState::setWorldTransform(const btTransform &trans)
{
	object->SetTransformWithoutRigidBody(tTransform::FromBtTransform(trans));
}




