
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
		GLuint depth_rbo;

		tCamera *camera;
		tRenderSpace *render_space;

		void GeometryPass(int side, tWorld *world, tRenderer *renderer);
		void LightPass(int side, tWorld *world, tRenderer *renderer);

	public:
		tCubeMapReflection(int resolution, tVector position);
		~tCubeMapReflection(void);

		void Render(tRenderer *renderer);

		tVector GetPosition(void)		{ return position; }
		GLuint GetCubeMapTexture(void)	{ return color_tex; }
};



#endif
