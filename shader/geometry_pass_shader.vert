#version 330

in vec3 vertex_attr;
in vec2 uv_attr;
in vec3 normal_attr;
in vec3 tang_attr;
in vec3 bitang_attr;

layout(std140) uniform MatrixBlock
{
	mat4 modelview_projection_matrix;
} matrix_uni;

uniform vec3 cam_pos_uni;

uniform mat4 transformation_uni;

out vec3 pos_var;
out vec3 normal_var;
out vec3 tang_var;
out vec3 bitang_var;
out vec2 uv_var;

out vec3 cam_dir_var;

out vec3 reflection_center_var;
out float reflection_radius_var;


void main(void)
{
	vec3 vertex_pos = vertex_attr;

	vec4 pos = vec4(vertex_pos, 1.0) * transformation_uni;
	pos_var = pos.xyz;
	normal_var = normalize(normal_attr * mat3(transformation_uni)); // TODO: correct transformation of normals
	tang_var = normalize(tang_attr * mat3(transformation_uni));
	bitang_var = normalize(bitang_attr * mat3(transformation_uni));
	uv_var = uv_attr;
	cam_dir_var = cam_pos_uni - pos.xyz;
	
	reflection_center_var = (vec4(0.0, 0.0, 0.0, 1.0) * transformation_uni).xyz;
	reflection_radius_var = 1.0;
	
	gl_Position = matrix_uni.modelview_projection_matrix * pos;
}