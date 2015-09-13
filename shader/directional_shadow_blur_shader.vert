#version 330

in vec2 vertex_attr;
in vec2 uv_coord_attr;

out vec2 uv_coord_var;

void main(void)
{
	uv_coord_var = uv_coord_attr;
	gl_Position = vec4(vertex_attr, 0.0, 1.0);
}
