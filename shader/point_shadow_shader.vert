#version 330

uniform mat4 modelview_projection_matrix_uni;

in vec3 vertex_attr;

uniform mat4 transformation_uni;

out vec3 pos_var;

void main(void)
{
	vec3 vertex_pos = vertex_attr;
		
	vec4 pos = vec4(vertex_pos, 1.0) * transformation_uni;
	pos_var = pos.xyz;
	
	gl_Position = modelview_projection_matrix_uni * pos;
}