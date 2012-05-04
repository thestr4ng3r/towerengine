#include "towerengine.h"

CAnimation::CAnimation(CMesh *mesh, const char *name, float len, int create_keyframes)
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

CAnimation::~CAnimation(void)
{
	while(key_first)
		delete key_first;

	mesh->RemoveAnimation(this);

}

int CAnimation::Play(float t, int loop)
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

void CAnimation::ChangeLength(float len)
{
	if(len > 0.0)
		this->len = len;
}

float CAnimation::GetLength(void)
{
	return len;
}

void CAnimation::SetTime(float time)
{
	if(time <= len)
		this->time = time;
}

void CAnimation::SetName(const char *name)
{
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
}

CKeyFrame *CAnimation::NewKeyFrame(float time)
{
	CKeyFrame *f;

	if(time < 0.0 || time > len)
		return 0;

	for(f=key_first; f; f=f->chain_next)
		if(f->time == time)
			return 0;

	return new CKeyFrame(this, time);
}

CKeyFrame *CAnimation::NewKeyFrameFromData(float time, int c, int *vert, CVector *vec)
{
	CKeyFrame *f;

	if(time < 0.0 || time > len)
		return 0;

	for(f=key_first; f; f=f->chain_next)
		if(f->time == time)
			return 0;

	f = new CKeyFrame(this, time);
	f->CopyFromData(c, vert, vec);
	return f;
}

void CAnimation::ApplyCurrentFrame(void)
{
	CKeyFrame *a, *b; // a = last keyframe; b = next keyframe
	CVertexPosition *av, *bv;
	CKeyFrame *f;
	float a_dist, b_dist;
	float t_dist;
	float mix;
	int one_pos; // if there is one position with the same time as the current time, then this will be one.
	
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

	if((!a || !b) && !one_pos)
		return;

	if(one_pos)
	{
		a->ApplyPosition();
		return;
	}

	mix = -a_dist / (b->time - a->time);

	for(av = a->pos_first; av; av = av->chain_next)
		for(bv = b->pos_first; bv; bv = bv->chain_next)
			if(av->v == bv->v)
			{
				av->ApplyMixedPosition(bv, mix);
				break;
			}
}


int CAnimation::Count(void)
{
	int i;
	CKeyFrame *f;

	i = 0;
	for(f=key_first; f; f=f->chain_next)
		i++;
	return i;
}

CAnimation *CAnimation::Copy(CMesh *m)
{
	CAnimation *r;
	r = new CAnimation(m, name, len, 0);

	CKeyFrame *k;
	for(k=key_first; k; k=k->chain_next)
		k->Copy(r);

	return r;
}

//------------------------------------------------------------

CKeyFrame::CKeyFrame(CAnimation *anim, float time) : CMeshPosition(anim->mesh)
{
	this->anim = anim;
	chain_next = anim->key_first;
	anim->key_first = this;

	this->time = time;
}

CKeyFrame::~CKeyFrame(void)
{
	CKeyFrame *k;

	if(anim->key_first == this)
		anim->key_first = chain_next;
	else
		for(k=anim->key_first; k; k=k->chain_next)
			if(k->chain_next == this)
				k->chain_next = chain_next;
}

CKeyFrame *CKeyFrame::Copy(CAnimation *a)
{
	CKeyFrame *r;

	r = new CKeyFrame(a, time);
	
	CVertexPosition *p;
	CVertex *v;
	for(p=pos_first; p; p=p->chain_next)
	{
		v = a->mesh->GetVertexByID(p->v->id);
		if(!v)
			continue;
		new CVertexPosition(r, v, p->p);
	}

	return r;

}
