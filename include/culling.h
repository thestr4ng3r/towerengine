
#ifndef _CULLING_H
#define _CULLING_H

/// Base class for different types of Culling Algorithms/Volumes
///
class tCulling
{
	public:
		virtual ~tCulling(void) {}

		/// Optional initialization method for the specific culling
		///
		virtual void InitCulling(void) {}

		/// Test whether a point should be culled
		/// \param point
		/// \return true if the point lies outside the culling and should not be rendered
		virtual bool TestPointCulling(tVector point) =0;

		/// Test whether a sphere should be culled
		/// \param point
		/// \return true if the sphere lies fully outside the culling and should not be rendered
		virtual bool TestSphereCulling(tVector center, float radius) =0;

		/// Test whether an axis aligned bounding box should be culled
		/// \param point
		/// \return true if the box lies fully outside the culling and should not be rendered
		virtual bool TestBoundingBoxCulling(tBoundingBox b) =0;
};

class tCullingCompound: public tCulling
{
	public:
		enum Operation { INTERSECTION, UNION };

	private:
		std::vector<tCulling *> cullings;
		Operation operation;

	public:
		tCullingCompound(Operation operation, tCulling *a = 0, tCulling *b = 0);
		virtual ~tCullingCompound() {}

		virtual void InitCulling(void);
		virtual bool TestPointCulling(tVector point);
		virtual bool TestSphereCulling(tVector center, float radius);
		virtual bool TestBoundingBoxCulling(tBoundingBox b);
};


#endif
