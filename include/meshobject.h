
#ifndef _MESHOBJECT_H
#define _MESHOBJECT_H


class tMeshObject : public tObject
{
	private:
		tMesh *mesh;

		bool visible;

		bool animation_mode;
		bool loop;
		tAnimation *animation;
		float time;

		char *pose;

		tVector pos;
		tVector x, y, z;
		tVector scale;
		tVector color;
		float alpha;

		tTransformationMatrix *transformation;

		float fade_end;
		float fade_speed;

	public:
		tMeshObject(tMesh *mesh, float mass = 0.0);

		void SetAnimation(const char *animation);
		void Play(float time);
		bool GetAnimationFinished(void);
		void SetAnimationLoop(bool l)					{ loop = l; }
		void SetAnimationMode(bool a)					{ animation_mode = a; }
		void SetAnimationTime(float t)					{ time = t; }
		void SetPose(const char *pose);
		void SetPosition(tVector v)						{ pos = v; }
		void SetXY(tVector x, tVector y)				{ SetXYZ(x, y, Cross(x, y)); }
		void SetYZ(tVector y, tVector z)				{ SetXYZ(Cross(y, z), y, z); }
		void SetXZ(tVector x, tVector z)				{ SetXYZ(x, Cross(x, z), z); }
		void SetXYZ(tVector x, tVector y, tVector z)	{ this->x = x; this->y = y; this->z = z; }
		//void SetRotation(CVector v)						{ rot = v; }
		void SetScale(tVector v)						{ scale = v; }
		void SetColor(tVector c)						{ color = c; }
		void SetAlpha(float a)							{ alpha = a; }
		void SetColor(tVector c, float a)				{ SetColor(c); SetAlpha(a); }
		void SetVisible(bool visible)					{ this->visible = visible; }
		void Fade(float fade_end, float time);

		tVector GetPosition(void)						{ return pos; }
		tVector GetX(void)								{ return x; }
		tVector GetY(void)								{ return y; }
		tVector GetZ(void)								{ return z; }

		void UpdateRigidBodyTransformation(void);

		void GeometryPass(void);
		void ForwardPass(void);
		tBoundingBox GetBoundingBox(void);
};

class tMeshObjectMotionState : public btMotionState
{
	private:
		tMeshObject *object;

	public:
		tMeshObjectMotionState(tMeshObject *object);

		virtual void getWorldTransform(btTransform &trans) const;
		virtual void setWorldTransform(const btTransform &trans);
};

#endif
