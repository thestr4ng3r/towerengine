
#ifndef _DIRECTIONAL_LIGHT_H
#define _DIRECTIONAL_LIGHT_H

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

		void InitShadow(int size, int splits = 4, bool blur_enabled = false, float blur_size = 0.003);
		void RenderShadow(CWorld *world);
};

#endif
