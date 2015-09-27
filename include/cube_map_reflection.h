
#ifndef _CUBE_MAP_REFLECTION_H
#define _CUBE_MAP_REFLECTION_H


class tCubeMapReflection
{
	private:
		tRenderer *renderer;

		tVector position;

		int resolution;
		tGBuffer *gbuffer;

		GLuint fbo;
		GLuint color_tex;

		tCamera *camera;
		tRenderSpace *render_space;

		tMatrixBuffer *matrix_buffer;

		bool invalid;

		void GeometryPass(int side, tWorld *world);
		void LightPass(int side, tWorld *world);

	public:
		tCubeMapReflection(tRenderer *renderer, int resolution, tVector position);
		~tCubeMapReflection(void);

		void Render(void);

		void Invalidate(void)			{ invalid = true; }

		tVector GetPosition(void)		{ return position; }
		GLuint GetCubeMapTexture(void)	{ return color_tex; }

		bool GetInvalid(void)			{ return invalid; }
};



#endif
