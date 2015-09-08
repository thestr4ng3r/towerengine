#version 450 core

in vec2 vertex_attr;

out vec2 uv_coord_var;

void main(void)
{
	uv_coord_var = (vertex_attr + 1.0) * 0.5;
	gl_Position = vec4(vertex_attr, 0.0, 1.0);
}

