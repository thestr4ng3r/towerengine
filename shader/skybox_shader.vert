#version 330

layout(std140) uniform MatrixBlock
{
	mat4 modelview_projection_matrix;
} matrix_uni;

uniform vec3 cam_pos_uni;

in vec3 vertex_attr;

out vec3 pos_var;

void main(void)
{
	pos_var = vertex_attr;
	gl_Position = matrix_uni.modelview_projection_matrix * vec4(cam_pos_uni + vertex_attr, 1.0);
}