#version 330

uniform mat4 modelview_projection_matrix_uni;

uniform vec3 cam_pos_uni;

in vec3 vertex_attr;

out vec3 pos_var;

void main(void)
{
	pos_var = vertex_attr;
	gl_Position = modelview_projection_matrix_uni * vec4(cam_pos_uni + vertex_attr, 1.0);
}