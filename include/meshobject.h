
#ifndef _MESHOBJECT_H
#define _MESHOBJECT_H


class CMeshObject : public CObject
{
	private:
		CMesh *mesh;

		bool visible;

		bool animation_mode;
		bool loop;
		CAnimation *animation;
		float time;

		char *pose;

		CVector pos;
		CVector x, y, z;
		CVector scale;
		CVector color;
		float alpha;

		CTransformationMatrix *transformation;

		float fade_end;
		float fade_speed;

	public:
		CMeshObject(CMesh *mesh, float mass = 0.0);

		void SetAnimation(const char *animation);
		void Play(float time);
		bool GetAnimationFinished(void);
		void SetAnimationLoop(bool l)					{ loop = l; }
		void SetAnimationMode(bool a)					{ animation_mode = a; }
		void SetAnimationTime(float t)					{ time = t; }
		void SetPose(const char *pose);
		void SetPosition(CVector v)						{ pos = v; }
		void SetXY(CVector x, CVector y)				{ SetXYZ(x, y, Cross(x, y)); }
		void SetYZ(CVector y, CVector z)				{ SetXYZ(Cross(y, z), y, z); }
		void SetXZ(CVector x, CVector z)				{ SetXYZ(x, Cross(x, z), z); }
		void SetXYZ(CVector x, CVector y, CVector z)	{ this->x = x; this->y = y; this->z = z; }
		//void SetRotation(CVector v)						{ rot = v; }
		void SetScale(CVector v)						{ scale = v; }
		void SetColor(CVector c)						{ color = c; }
		void SetAlpha(float a)							{ alpha = a; }
		void SetColor(CVector c, float a)				{ SetColor(c); SetAlpha(a); }
		void SetVisible(bool visible)					{ this->visible = visible; }
		void Fade(float fade_end, float time);

		CVector GetPosition(void)						{ return pos; }
		CVector GetX(void)								{ return x; }
		CVector GetY(void)								{ return y; }
		CVector GetZ(void)								{ return z; }

		void UpdateRigidBodyTransformation(void);

		void PutToGL(void);
		CBoundingBox GetBoundingBox(void);
};

class CMeshObjectMotionState : public btMotionState
{
	private:
		CMeshObject *object;

	public:
		CMeshObjectMotionState(CMeshObject *object);

		virtual void getWorldTransform(btTransform &trans) const;
		virtual void setWorldTransform(const btTransform &trans);
};

#endif
