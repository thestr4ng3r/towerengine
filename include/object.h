
#ifndef _OBJECT_H
#define _OBJECT_H

class tObject
{
	private:
	//	string tag;
		tWorld *world;

	protected:
		virtual void AddedToWorld(tWorld *world) {}
		virtual void RemovedFromWorld(tWorld *world) {}

	public:
		tObject(void);
		virtual ~tObject(void) {}

		virtual void DepthPrePass(tRenderer *) {}
		virtual void GeometryPass(tRenderer *) {}
		virtual void ForwardPass(tRenderer *) {}
		virtual void RefractionPass(tRenderer *) {}
		virtual tBoundingBox GetBoundingBox(void) = 0;

		void AddedObjectToWorld(tWorld *world);
		void RemovedObjectFromWorld(tWorld *world);

		tWorld *GetWorld(void)	{ return world; }

		virtual bool GetCubeMapReflectionEnabled(void)	{ return false; }
};

class tTransformObject : public tObject
{
	protected:
		tTransform transform;

		virtual void TransformChanged(void) {}

	public:
		tTransformObject(void)						{ this->transform = tTransform::GetIdentity(); }
		tTransformObject(tTransform transform)		{ this->transform = transform; }

		tTransform GetTransform(void)				{ return transform; }
		void SetTransform(tTransform transform);
};


#endif
