
#ifndef _TOWERENGINE_FORWARD_RENDERER_H
#define _TOWERENGINE_FORWARD_RENDERER_H

class tForwardRenderer: public tRenderer
{
	private:
		void DepthPrePass();

	protected:
		tStandardForwardShader *standard_shader;

		tLightingShaderPointLightsBuffer *point_lights_buffer;

		void InitForwardRenderer(tWorld *world);

		void PrepareRender(tCamera *camera, tRenderSpace *render_space);

		void RenderForward();

	public:
		virtual ~tForwardRenderer();
};

#endif
