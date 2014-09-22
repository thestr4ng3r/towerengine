
#ifndef _CAMERA_H
#define _CAMERA_H

class CCamera
{
	private:
		CVector pos;
		CVector dir;
		CVector up;
		float angle;
		float aspect;
		float near_clip, far_clip;

		CVector frustum_planes_points[6];
		CVector frustum_planes_normals[6];

	public:
		CCamera(void);

		CVector GetPosition(void)		{ return pos; }
		CVector GetDirection(void)		{ return dir; }
		CVector GetUp(void)				{ return up; }
		float GetAngle(void)			{ return angle; }
		float GetAspect(void)			{ return aspect; }
		float GetNearClip(void)			{ return near_clip; }
		float GetFarClip(void)			{ return far_clip; }

		void CalculateFrustumPlanes(void);
		CVector *GetRelativeFrustumCorners(float near, float far);
		CVector *GetRelativeFrustumCorners(void) { return GetRelativeFrustumCorners(near_clip, far_clip); }
		CVector *GetFrustumPlanesPoints(void)	{ return frustum_planes_points; }
		CVector *GetFrustumPlanesNormals(void)	{ return frustum_planes_normals; }
		CVector *GetViewRays(void);

		bool TestPointCulling(CVector point);
		bool TestSphereCulling(CVector center, float radius);
		bool TestBoundingBoxCulling(CBoundingBox b);

		void SetPosition(CVector p)		{ pos = p; }
		void SetDirection(CVector d)	{ dir = d; }
		void SetUp(CVector t)			{ up = t; }
		void SetAngle(float a)			{ angle = a; }
		void SetAspect(float a)			{ aspect = a; }
		void SetClip(float n, float f)	{ near_clip = n; far_clip = f; }
		void SetNearClip(float n)		{ near_clip = n; }
		void SetFarClip(float f)		{ far_clip = f; }

		void SetModelviewProjectionMatrix(void);
};

#endif
