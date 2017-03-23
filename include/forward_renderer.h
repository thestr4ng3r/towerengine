
#ifndef PSYCHOSIS_FORWARD_RENDERER_H
#define PSYCHOSIS_FORWARD_RENDERER_H

class tForwardRenderer
{
	protected:
		tStandardForwardShader *standard_shader;

		void InitRenderer();

	public:
		virtual ~tForwardRenderer();
};

#endif
