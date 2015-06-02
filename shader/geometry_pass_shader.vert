#version 330

in vec3 vertex_attr;
in vec3 vertex2_attr; // vertex of next keyframe
in vec2 uv_attr;
in vec3 normal_attr;
in vec3 tang_attr;
in vec3 bitang_attr;

uniform mat4 modelview_matrix_uni;
uniform mat4 projection_matrix_uni;

uniform float vertex_mix_uni;
uniform mat4 transformation_uni;

out vec3 pos_var;
out vec3 normal_var;
out vec3 tang_var;
out vec3 bitang_var;
out vec2 uv_var;

out vec3 cam_dir_var;


void main(void)
{
	vec3 vertex_pos = vertex_attr;
	if(vertex_mix_uni > 0.0)
		vertex_pos = vertex_pos * (1.0 - vertex_mix_uni) + vertex2_attr * vertex_mix_uni;
	vec4 pos = vec4(vertex_pos, 1.0) * transformation_uni;
	pos_var = pos.xyz;
	normal_var = normalize(normal_attr * mat3(transformation_uni));
	tang_var = normalize(tang_attr * mat3(transformation_uni));
	bitang_var = normalize(bitang_attr * mat3(transformation_uni));
	uv_var = uv_attr;
	cam_dir_var = inverse(modelview_matrix_uni)[3].xyz - pos.xyz;
	
	gl_Position = (projection_matrix_uni * modelview_matrix_uni) * pos;
}