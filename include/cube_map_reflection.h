
#ifndef _CUBE_MAP_REFLECTION_H
#define _CUBE_MAP_REFLECTION_H


class tCubeMapReflection
{
	private:
		tVector position;

		int resolution;
		tGBuffer *gbuffer;

		GLuint fbo;
		GLuint color_tex;

		tCamera *camera;
		tRenderSpace *render_space;

		bool initialized;
		bool invalid;

		void GeometryPass(tRenderer *renderer, int side, tWorld *world);
		void LightPass(tRenderer *renderer, int side, tWorld *world);

	public:
		tCubeMapReflection(tVector position);
		~tCubeMapReflection(void);

		void Init(int resolution);

		void Render(tRenderer *renderer);

		void Invalidate(void)			{ invalid = true; }

		tVector GetPosition(void)		{ return position; }
		GLuint GetCubeMapTexture(void)	{ return color_tex; }

		bool GetInvalid(void)			{ return invalid; }
};



#endif
