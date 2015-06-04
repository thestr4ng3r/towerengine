#version 330

uniform mat4 modelview_projection_matrix_uni;

in vec3 vertex_attr;
in vec3 vertex2_attr; // vertex of next keyframe

uniform float vertex_mix_uni;
uniform mat4 transformation_uni;

out vec3 pos_var;

void main(void)
{
	vec3 vertex_pos = vertex_attr;
	if(vertex_mix_uni > 0.0)
		vertex_pos = vertex_pos * (1.0 - vertex_mix_uni) + vertex2_attr * vertex_mix_uni;
		
	vec4 pos = vec4(vertex_pos, 1.0) * transformation_uni;
	pos_var = pos.xyz;
	
	gl_Position = modelview_projection_matrix_uni * pos;
}