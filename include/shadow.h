
#ifndef _SHADOW_H
#define _SHADOW_H


class CPointLightShadow
{
	private:
		CPointLight *light;

		GLuint tex;
		GLuint fbo;
		GLuint depth_rbo;

		int size;

	public:
		CPointLightShadow(CPointLight *light, int size);

		void Render(CWorld *world);

		GLuint GetShadowMap(void)	{ return tex; }
};

class CDirectionalLightShadow
{

};



#endif
