
#ifndef _CAMERA_H
#define _CAMERA_H

class tCamera
{
	private:
		tVector pos;
		tVector dir;
		tVector up;
		float angle;
		float aspect;
		float near_clip, far_clip;

		tVector frustum_planes_points[6];
		tVector frustum_planes_normals[6];

	public:
		tCamera(void);

		tVector GetPosition(void)		{ return pos; }
		tVector GetDirection(void)		{ return dir; }
		tVector GetUp(void)				{ return up; }
		float GetAngle(void)			{ return angle; }
		float GetAspect(void)			{ return aspect; }
		float GetNearClip(void)			{ return near_clip; }
		float GetFarClip(void)			{ return far_clip; }

		void CalculateFrustumPlanes(void);
		tVector *GetRelativeFrustumCorners(float near, float far);
		tVector *GetRelativeFrustumCorners(void) { return GetRelativeFrustumCorners(near_clip, far_clip); }
		tVector *GetFrustumPlanesPoints(void)	{ return frustum_planes_points; }
		tVector *GetFrustumPlanesNormals(void)	{ return frustum_planes_normals; }
		tVector *GetViewRays(void);
		tVector GetScreenRay(float x, float y);
		tVector2 GetProjectedPoint(tVector point);

		bool TestPointCulling(tVector point);
		bool TestSphereCulling(tVector center, float radius);
		bool TestBoundingBoxCulling(tBoundingBox b);

		void SetPosition(tVector p)		{ pos = p; }
		void SetDirection(tVector d)	{ dir = d; }
		void SetUp(tVector t)			{ up = t; }
		void SetAngle(float a)			{ angle = a; }
		void SetAspect(float a)			{ aspect = a; }
		void SetClip(float n, float f)	{ near_clip = n; far_clip = f; }
		void SetNearClip(float n)		{ near_clip = n; }
		void SetFarClip(float f)		{ far_clip = f; }

		void SetModelviewProjectionMatrix(void);
};

#endif
