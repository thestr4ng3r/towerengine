#version 330

in vec3 vertex_attr;
in vec2 uv_attr;

uniform mat4 modelview_projection_matrix_uni;

uniform vec3 cam_pos_uni;

uniform float vertex_mix_uni;
uniform mat4 transformation_uni;

out vec2 uv_var;


void main(void)
{
	vec3 vertex_pos = vertex_attr;

	vec4 pos = vec4(vertex_pos, 1.0) * transformation_uni;
	
	uv_var = uv_attr;
	
	gl_Position = modelview_projection_matrix_uni * pos;
}