
#include "towerengine.h"


CMeshObject::CMeshObject(CMesh *mesh)
{
	this->mesh = mesh;
	animation = 0;
	pose = cstr("Idle");
	animation_mode = 0;
	loop = false;
	pos = Vec(0.0, 0.0, 0.0);
	rot = Vec(0.0, 0.0, 0.0);
	x = Vec(1.0, 0.0, 0.0);
	y = Vec(0.0, 1.0, 0.0);
	z = Vec(0.0, 0.0, 1.0);
	scale = Vec(1.0, 1.0, 1.0);
	color = Vec(1.0, 1.0, 1.0);
	alpha = 1.0;
}

void CMeshObject::SetAnimation(const char *animation)
{
	CAnimation *a = mesh->GetAnimationByName(animation);
	if(a != this->animation)
		time = 0.0;
	this->animation = a;
}

void CMeshObject::PlayAnimation(float time)
{
	if(!animation_mode || !animation)
		return;

	if(!loop)
		this->time = min(animation->GetLength(), this->time + time);
	else
		this->time = fmod(this->time + time, animation->GetLength());
}

bool CMeshObject::GetAnimationFinished(void)
{
	if(!animation_mode || !animation)
		return true;

	return !loop && this->time >= animation->GetLength();
}

void CMeshObject::SetPose(const char *pose)
{
	if(this->pose)
		delete [] this->pose;
	this->pose = cstr(pose);
}

void CMeshObject::PutToGL(CVector cam)
{
	CMesh::LoadIdentity();
	CMesh::Translate(pos);
	CMesh::Scale(scale);
	CMesh::Rotate(rot);
	CMesh::SetXYZ(x, y, z);
	CMesh::Color(color, alpha);

	if(animation_mode && animation)
	{
		mesh->ChangeAnimation(animation);
		mesh->SetAnimationLoop(0);
		animation->SetTime(time);
		mesh->ApplyAnimation();
	}
	else
	{
		if(pose)
			mesh->ChangePosition(pose);
		else
			mesh->ChangePosition("Idle");
	}
	mesh->PutToGL(cam);
}
