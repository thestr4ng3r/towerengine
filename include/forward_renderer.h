
#ifndef _TOWERENGINE_FORWARD_RENDERER_H
#define _TOWERENGINE_FORWARD_RENDERER_H

class tForwardRenderer: public tRenderer
{
	protected:
		tStandardForwardShader *standard_shader;

		tLightingShaderPointLightsBuffer *point_lights_buffer;

		void InitForwardRenderer(tWorld *world);

		void PrepareRender(tCamera *camera, tRenderSpace *render_space);

		void StandardForwardPass();

	public:
		virtual ~tForwardRenderer();
};

#endif
