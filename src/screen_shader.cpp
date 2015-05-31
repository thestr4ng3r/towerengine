
#include "towerengine.h"
#include "tresources.h"

void tScreenShader::InitScreenShader(const char *frag_src, const char *shader_name)
{
	InitShader(screen_shader_vert, frag_src, shader_name);
	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	LinkProgram();
}
