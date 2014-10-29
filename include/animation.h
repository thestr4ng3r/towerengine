#ifndef _ANIMATION_H
#define _ANIMATION_H

class tAnimation
{
	private:
		float len; // in seconds
		float time;

		char *name;

	public:
		tMesh *mesh;
		tAnimation *chain_next;

		tKeyFrame *key_first;

		tAnimation(tMesh *mesh, const char *name, float len = 1.0, int create_keyframes = 1);
		~tAnimation(void);

		int Play(float time, int loop = 1); // returns 1 if loop == 0 and the animation is finished

		void ChangeLength(float len);
		void SetTime(float time);
		void SetName(const char *name);

		tKeyFrame *NewKeyFrame(float time);
		tKeyFrame *NewKeyFrameFromData(float time, int c, int *vert, tVector *vec);

		void GetKeyframePair(tKeyFrame **a, tKeyFrame **b, float *mix);

		void ApplyCurrentFrame(void);

		float GetLength(void);
		int Count(void);
		float GetTime(void) { return time; };
		char *GetName(void) { return cstr(name); };

		tAnimation *Copy(tMesh *m);
};

struct tKeyFrame : tMeshPose
{
	tAnimation *anim;
	tKeyFrame *chain_next;
	float time;

	tKeyFrame(tAnimation *anim, float time = 0.0);
	virtual ~tKeyFrame(void);

	virtual tKeyFrame *Copy(tAnimation *a);
};

#endif
