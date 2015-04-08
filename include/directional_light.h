
#ifndef _DIRECTIONAL_LIGHT_H
#define _DIRECTIONAL_LIGHT_H

class tDirectionalLight
{
	private:
		tVector dir;
		tVector color;

		bool shadow_enabled;
		tDirectionalLightShadow *shadow;

	public:
		tDirectionalLight(tVector dir, tVector color);

		void SetDirection(tVector dir)	{ this->dir = dir; this->dir.Normalize(); }
		void SetColor(tVector color)	{ this->color = color; }

		tVector GetDirection(void)	{ return dir; }
		tVector GetColor(void)		{ return color; }
		bool GetShadowEnabled(void)			{ return shadow_enabled; }
		tDirectionalLightShadow *GetShadow(void)	{ return shadow; }

		void InitShadow(int size, int splits = 4, bool blur_enabled = false, float blur_size = 0.003);
		void RenderShadow(tRenderer *renderer);

		/*
		 * tDirectionalLightShader must be bound and its GBuffer Uniform set before!
		 */
		void InitRenderLighting(tDirectionalLightingShader *shader);
};

#endif
