#version 330

uniform vec2 vertex_transform_uni;

in vec2 vertex_attr;
in vec2 uv_attr;
in vec4 color_attr;

out vec2 uv_var;
out vec4 color_var;

void main(void)
{
	uv_var = uv_attr;
	color_var = color_attr;

	vec2 pos = vertex_attr * vertex_transform_uni + vec2(-1.0, 1.0);
	gl_Position = vec4(pos, 0.0, 1.0);
}