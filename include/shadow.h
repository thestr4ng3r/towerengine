
#ifndef _SHADOW_H
#define _SHADOW_H


class CPointLightShadow
{
	private:
		CPointLight *light;

		GLuint tex;
		GLuint fbo;

		int size;

		float near_clip;

	public:
		CPointLightShadow(CPointLight *light, int size, float near_clip);

		void Render(CWorld *world);

		GLuint GetShadowMap(void)	{ return tex; }
		float GetNearClip(void)		{ return near_clip; }
};

class CDirectionalLightShadow
{

};



#endif
