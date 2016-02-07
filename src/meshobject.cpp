
#include "towerengine.h"

#include <BulletCollision/CollisionShapes/btShapeHull.h>

using namespace std;

tMeshObject::tMeshObject(tMesh *mesh) : tTransformObject()
{
	this->mesh = mesh;
	rigid_body = 0;
	motion_state = 0;
	animation = 0;
	pose = "Idle";
	animation_mode = 0;
	loop = false;
	color = Vec(1.0, 1.0, 1.0);
	alpha = 1.0;
	visible = true;
	time = 0.0;
	motion_state = new tMeshObjectMotionState(this);

	rigid_body = 0;
	collision_shape = 0;

	replace_materials = 0;

	cube_map_reflection = 0;
}

tMeshObject::~tMeshObject(void)
{
	delete replace_materials;
	delete motion_state;

	delete rigid_body;
	delete collision_shape;
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

void tMeshObject::SetPose(string pose)
{
	this->pose = pose;
}

tBoundingBox tMeshObject::GetBoundingBox(void)
{
	tBoundingBox b;
	tVector p[8];
	mesh->GetBoundingBox().GetCornerPoints(p);

	//btTransform trans(btMatrix3x3(mat[0], mat[1], mat[2], mat[4], mat[5], mat[6], mat[8], mat[9], mat[10]), btVector3(mat[3], mat[7], mat[11]));
	//rigid_body->setWorldTransform(trans);

	for(int i=0; i<8; i++)
		b.AddPoint(p[i] * transform);

	return b;
}

void tMeshObject::ReplaceMaterial(string name, tMaterial *mat)
{
	if(!replace_materials)
		replace_materials = new map<tMaterial *, tMaterial *>();

	tMaterial *original_mat = mesh->GetMaterialByName(name);
	if(!original_mat)
		return;

	(*replace_materials)[original_mat] = mat;
}

void tMeshObject::RestoreReplaceMaterial(string name)
{
	if(!replace_materials)
		return;

	tMaterial *original_mat = mesh->GetMaterialByName(name);
	if(!original_mat)
		return;

	map<tMaterial *, tMaterial *>::iterator i = replace_materials->find(original_mat);
	if(i != replace_materials->end())
		replace_materials->erase(i);
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
		mesh->ChangePose(pose);
	}
	mesh->DepthPrePass(renderer, replace_materials);
}

void tMeshObject::GeometryPass(tRenderer *renderer, bool cube_map_reflection_enabled)
{
	if(!visible || alpha <= 0.0 || !mesh)
		return;

	float *temp = transform.GetMatrix(transform_matrix);

	renderer->GetCurrentFaceShader()->SetTransformation(temp);
	if(cube_map_reflection_enabled && cube_map_reflection)
		renderer->GetCurrentFaceShader()->SetCubeMapReflectionTexture(cube_map_reflection->GetCubeMapTexture());

	if(animation_mode && animation)
	{
		mesh->ChangeAnimation(animation);
		mesh->SetAnimationLoop(0);
		animation->SetTime(time);
	}
	else
	{
		mesh->ChangePose(pose);
	}
	mesh->GeometryPass(renderer, replace_materials);
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
		mesh->ChangePose(pose);
	}

	float *temp = transform.GetMatrix(transform_matrix);
	mesh->ForwardPass(renderer, temp, replace_materials);
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
		mesh->ChangePose(pose);
	}

	float *temp = transform.GetMatrix(transform_matrix);
	mesh->RefractionPass(renderer, temp, replace_materials);
}


void tMeshObject::InitMeshRigidBody(float mass, bool convex, bool hull_enabled)
{
	if(rigid_body)
		return;

	btVector3 inertia(0.0, 0.0, 0.0);

	if(convex || mass > 0.0)
	{
		if(mesh->GetPhysicsMesh())
		{
			btConvexTriangleMeshShape *original_shape = new btConvexTriangleMeshShape(mesh->GetPhysicsMesh());
			original_shape->setMargin(0.01);

			if(hull_enabled)
			{
				btShapeHull *hull = new btShapeHull(original_shape);
				hull->buildHull(original_shape->getMargin());

				btConvexHullShape *hull_shape = new btConvexHullShape();

				for (int i = 0; i < hull->numVertices(); i++)
					hull_shape->addPoint(hull->getVertexPointer()[i], false);
				hull_shape->recalcLocalAabb();

				delete original_shape;
				delete hull;

				collision_shape = hull_shape;
			}
			else
				collision_shape = original_shape;

			collision_shape->setMargin(0.01);
			collision_shape->calculateLocalInertia(mass, inertia);
		}
		else
			collision_shape = new btEmptyShape();
	}
	else
	{
		if(mesh->GetPhysicsMesh())
			collision_shape = new btBvhTriangleMeshShape(mesh->GetPhysicsMesh(), true);
		else
			collision_shape = new btEmptyShape();

		collision_shape->setMargin(0.01);
	}

	CreateRigidBody(mass, inertia);
}


void tMeshObject::InitBoxRigidBody(tVector half_extents, float mass)
{
	if(rigid_body)
		return;

	collision_shape = new btBoxShape(BtVec(half_extents));

	btVector3 inertia;
	collision_shape->calculateLocalInertia(mass, inertia);

	collision_shape->setMargin(0.01);

	CreateRigidBody(mass, inertia);
}

void tMeshObject::CreateRigidBody(btScalar mass, btVector3 inertia)
{
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion_state, collision_shape, inertia);

	rigid_body = new btRigidBody(info);
	rigid_body->setUserPointer(this);

	tWorld *world = GetWorld();
	if(world)
		world->GetDynamicsWorld()->addRigidBody(rigid_body);
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




