
#ifndef _LIGHT_H
#define _LIGHT_H


class CPointLight
{
	private:
		CVector pos;
		CVector color;
		float distance;

		bool shadow_enabled;
		CPointLightShadow *shadow;

	public:
		CPointLight(CVector pos, CVector color, float distance);

		void SetPosition(CVector pos)		{ this->pos = pos; }
		void SetColor(CVector color)	{ this->color = color; }
		void SetDistance(float dist)	{ this->distance = dist; }

		CVector GetPosition(void)			{ return pos; }
		CVector GetColor(void)				{ return color; }
		float GetDistance(void)				{ return distance; }
		bool GetShadowEnabled(void)			{ return shadow_enabled; }
		CPointLightShadow *GetShadow(void)	{ return shadow; }

		void InitShadow(int size, bool blur_enabled = false, float blur_size = 0.05);
		void RenderShadow(CWorld *world);
};

class CDirectionalLight
{
	private:
		CVector dir;
		CVector color;

		bool shadow_enabled;
		CDirectionalLightShadow *shadow;

	public:
		CDirectionalLight(CVector dir, CVector color);

		void SetDirection(CVector dir)	{ this->dir = dir; this->dir.Normalize(); }
		void SetColor(CVector color)	{ this->color = color; }

		CVector GetDirection(void)	{ return dir; }
		CVector GetColor(void)		{ return color; }
		bool GetShadowEnabled(void)			{ return shadow_enabled; }
		CDirectionalLightShadow *GetShadow(void)	{ return shadow; }

		void InitShadow(int size, int splits = 4);
		void RenderShadow(CWorld *world);
};


#endif
