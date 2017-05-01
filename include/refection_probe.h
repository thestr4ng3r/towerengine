
#ifndef _TOWERENGINE_REFLECTION_PROBE_REFLECTION_H
#define _TOWERENGINE_REFLECTION_PROBE_REFLECTION_H


class tReflectionProbe: public tObject
{
	private:
		tVector position;
		tAABB extent;

		unsigned int resolution;
		unsigned int resolution_log;
		tGBuffer *gbuffer;

		GLuint fbo;
		GLuint color_tex;

		GLuint blur_fbo;
		GLuint blur_tex;

		tCamera *camera;
		tRenderObjectSpace *render_space;

		bool initialized;
		bool invalid;

		void RenderGeometryPass(tDeferredRenderer *renderer, int side, tWorld *world);
		void RenderLightPass(tDeferredRenderer *renderer, int side, tWorld *world);
		void RenderBlurPass(tDeferredRenderer *renderer);

	public:
		tReflectionProbe(tVector position, tVector extent_a, tVector extend_b);
		~tReflectionProbe(void);

		void Init(unsigned int resolution_log);

		void Render(tDeferredRenderer *renderer);

		void Invalidate(void)			{ invalid = true; }

		tVector GetPosition(void)		{ return position; }
		tAABB GetExtent(void)	{ return extent; }

		GLuint GetCubeMapTexture(void)	{ return color_tex; }

		bool GetInvalid(void)			{ return invalid; }
};



#endif
