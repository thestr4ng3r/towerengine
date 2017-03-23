
#ifndef _CUBE_MAP_REFLECTION_H
#define _CUBE_MAP_REFLECTION_H


class tCubeMapReflection
{
	private:
		tVector position;
		tBoundingBox extent;

		unsigned int resolution;
		unsigned int resolution_log;
		tGBuffer *gbuffer;

		GLuint fbo;
		GLuint color_tex;

		GLuint blur_fbo;
		GLuint blur_tex;

		tCamera *camera;
		tRenderSpace *render_space;

		bool initialized;
		bool invalid;

		void GeometryPass(tDeferredRenderer *renderer, int side, tWorld *world);
		void LightPass(tDeferredRenderer *renderer, int side, tWorld *world);
		void BlurPass(tDeferredRenderer *renderer);

	public:
		tCubeMapReflection(tVector position, tVector extent_a, tVector extend_b);
		~tCubeMapReflection(void);

		void Init(unsigned int resolution_log);

		void Render(tDeferredRenderer *renderer);

		void Invalidate(void)			{ invalid = true; }

		tVector GetPosition(void)		{ return position; }
		tBoundingBox GetExtent(void)	{ return extent; }

		GLuint GetCubeMapTexture(void)	{ return color_tex; }

		bool GetInvalid(void)			{ return invalid; }
};



#endif
