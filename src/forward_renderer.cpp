
#include "towerengine.h"

void tForwardRenderer::InitRenderer()
{
	standard_shader = new tStandardForwardShader();
}

tForwardRenderer::~tForwardRenderer()
{
	delete standard_shader;
}