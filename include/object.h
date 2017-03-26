
#ifndef _OBJECT_H
#define _OBJECT_H

class tObject
{
	private:
		tWorld *world;

	protected:
		virtual void AddedToWorld(tWorld *world) {}
		virtual void RemovedFromWorld(tWorld *world) {}

	public:
		tObject(void);
		virtual ~tObject(void) {}

		virtual void DepthPrePass(tRenderer *) {}
		virtual void ShadowPass(tRenderer *) {}
		virtual void GeometryPass(tDeferredRenderer *, bool cube_map_reflection_enabled) {}
		virtual void ForwardPass(tDeferredRenderer *) {}
		virtual void RefractionPass(tDeferredRenderer *) {}
		virtual void StandardForwardPass(tForwardRenderer *) {}
		virtual tBoundingBox GetBoundingBox(void) = 0;

		void AddedObjectToWorld(tWorld *world);
		void RemovedObjectFromWorld(tWorld *world);

		tWorld *GetWorld(void)	{ return world; }
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
