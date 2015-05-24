#include "towerengine.h"

using namespace std;

tAnimation::tAnimation(tMesh *mesh, const char *name, float len, int create_keyframes)
{
	this->mesh = mesh;
	mesh->AddAnimation(this);

	key_first = 0;

	if(len > 0.0)
		this->len = len;
	else
		this->len = 1.0;

	SetName(name);

	if(create_keyframes)
	{
		NewKeyFrame(0.0);
		NewKeyFrame(len);
	}
}

tAnimation::~tAnimation(void)
{
	while(key_first)
		delete key_first;

	mesh->RemoveAnimation(this);


}

int tAnimation::Play(float t, int loop)
{
	float tp;
	int r = 0;
	tp = time + t;

	if(loop)
	{
		while(tp > len)
			tp-=len;
		while(tp < 0.0)
			tp+=len;
	}
	else
	{
		if(tp > len)
		{
			tp = len;
			r = 1;
		}
		if(tp < 0.0)
		{
			tp = 0.0;
			r = 1;
		}
	}
	SetTime(tp);

	return r;
}

void tAnimation::ChangeLength(float len)
{
	if(len > 0.0)
		this->len = len;
}

float tAnimation::GetLength(void)
{
	return len;
}

void tAnimation::SetTime(float time)
{
	if(time <= len)
		this->time = time;
}

void tAnimation::SetName(const char *name)
{
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
}

tKeyFrame *tAnimation::NewKeyFrame(float time)
{
	tKeyFrame *f;

	if(time < 0.0 || time > len)
		return 0;

	for(f=key_first; f; f=f->chain_next)
		if(f->time == time)
			return 0;

	return new tKeyFrame(this, time);
}

tKeyFrame *tAnimation::NewKeyFrameFromData(float time, int c, int *vert, tVector *vec)
{
	tKeyFrame *f;

	if(time < 0.0 || time > len)
		return 0;

	for(f=key_first; f; f=f->chain_next)
		if(f->time == time)
			return 0;

	f = new tKeyFrame(this, time);
	f->CopyFromData(c, vert, vec);
	return f;
}

void tAnimation::ApplyCurrentFrame(void)
{
	tKeyFrame *a, *b;
	float mix;

	GetKeyframePair(&a, &b, &mix);

	a->ApplyMixedPoseToVertices(b, mix);
}

void tAnimation::GetKeyframePair(tKeyFrame **r_a, tKeyFrame **r_b, float *mix)
{
	tKeyFrame *a, *b; // a = last keyframe; b = next keyframe
	tKeyFrame *f;
	float a_dist, b_dist;
	float t_dist;
	int one_pos; // if there is one pose with the same time as the current time, then this will be one.
	
	a_dist = -INFINITY;
	b_dist = INFINITY;
	a = 0;
	b = 0;
	one_pos = 0;

	for(f = key_first; f; f = f->chain_next) // search for the nearest keyframes
	{
		t_dist = f->time - time;
		if(t_dist == 0.0)
		{
			one_pos = 1;
			a = f;
			break;
		}
		else if(t_dist < 0.0)
		{
			if(t_dist > a_dist)
			{
				a_dist = t_dist;
				a = f;
			}
		}
		else if(t_dist > 0.0)
		{
			if(t_dist < b_dist)
			{
				b_dist = t_dist;
				b = f;
			}
		}
	}

	if(one_pos)
	{
		b = 0;
		*mix = 0.0;
	}
	else
		*mix = -a_dist / (b->time - a->time);
	*r_a = a;
	*r_b = b;
}

int tAnimation::Count(void)
{
	int i;
	tKeyFrame *f;

	i = 0;
	for(f=key_first; f; f=f->chain_next)
		i++;
	return i;
}

tAnimation *tAnimation::Copy(tMesh *m)
{
	tAnimation *r;
	r = new tAnimation(m, name, len, 0);

	tKeyFrame *k;
	for(k=key_first; k; k=k->chain_next)
		k->Copy(r);

	return r;
}

//------------------------------------------------------------

tKeyFrame::tKeyFrame(tAnimation *anim, float time) : tMeshPose(anim->mesh)
{
	this->anim = anim;
	chain_next = anim->key_first;
	anim->key_first = this;

	this->time = time;
}

tKeyFrame::~tKeyFrame(void)
{
	tKeyFrame *k;

	if(anim->key_first == this)
		anim->key_first = chain_next;
	else
		for(k=anim->key_first; k; k=k->chain_next)
			if(k->chain_next == this)
				k->chain_next = chain_next;
}

tKeyFrame *tKeyFrame::Copy(tAnimation *a)
{
	tKeyFrame *r;
	map<tVertex *, tVector>::iterator i;

	r = new tKeyFrame(a, time);

	for(i=vertices.begin(); i!=vertices.end(); i++)
		r->vertices.insert(pair<tVertex *, tVector>(i->first, i->second));

	return r;

}
