
#include "towerengine.h"
#include "resources.h"

void tNoOpShader::Init(void)
{
	InitShader(resources_get("no_op_shader.vert"), resources_get("no_op_shader.frag"));
	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	LinkProgram();
}