
#include "towerengine.h"

void tLightingShader::InitLightingShader(const char *frag_src, const char *shader_name)
{
	InitShader(light_pass_shader_vert, frag_src, "Point Lighting Shader");
	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	LinkProgram();
}
