#version 330

in vec3 vertex_attr;

uniform mat4 modelview_projection_matrix_uni;

void main(void)
{	
	gl_PointSize = 8.0;
	gl_Position = modelview_projection_matrix_uni * vec4(vertex_attr, 1.0);
}