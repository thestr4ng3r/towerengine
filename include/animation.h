#ifndef _ANIMATION_H
#define _ANIMATION_H

class CAnimation
{
	private:
		float len; // in seconds
		float time;

		char *name;

	public:
		CMesh *mesh;
		CAnimation *chain_next;

		CKeyFrame *key_first;

		CAnimation(CMesh *mesh, const char *name, float len = 1.0, int create_keyframes = 1);
		~CAnimation(void);

		int Play(float time, int loop = 1); // returns 1 if loop == 0 and the animation is finished

		void ChangeLength(float len);
		void SetTime(float time);
		void SetName(const char *name);

		CKeyFrame *NewKeyFrame(float time);
		CKeyFrame *NewKeyFrameFromData(float time, int c, int *vert, CVector *vec);

		void ApplyCurrentFrame(void);

		float GetLength(void);
		int Count(void);
		float GetTime(void) { return time; };
		char *GetName(void) { return cstr(name); };

		CAnimation *Copy(CMesh *m);
};

struct CKeyFrame : CMeshPosition
{
	CAnimation *anim;
	CKeyFrame *chain_next;
	float time;

	CKeyFrame(CAnimation *anim, float time = 0.0);
	virtual ~CKeyFrame(void);

	virtual CKeyFrame *Copy(CAnimation *a);
};

#endif
