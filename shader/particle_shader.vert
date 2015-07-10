#version 330

uniform mat4 modelview_matrix_uni;

in vec3 vertex_attr;
in float size_attr;

out float size_var;

void main(void)
{
	size_var = size_attr;
	gl_Position = modelview_matrix_uni * vec4(vertex_attr, 1.0);
}