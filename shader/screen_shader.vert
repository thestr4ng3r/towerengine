#version 330

in vec2 vertex_attr;

out vec2 uv_coord_var;

void main(void)
{
	uv_coord_var = vertex_attr;
	gl_Position = vec4(vertex_attr * 2.0 - 1.0, 0.0, 1.0);
}

