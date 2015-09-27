#version 330

in vec3 vertex_attr;
in vec2 uv_attr;

uniform mat4 modelview_projection_matrix_uni;

uniform mat4 transformation_uni;

uniform vec3 light_dir_uni;
uniform vec2 clip_uni;
uniform vec3 cam_pos_uni;

out float moment1_var;
out vec2 uv_var;

void main(void)
{
	vec3 vertex_pos = vertex_attr;

	vec4 pos = vec4(vertex_pos, 1.0) * transformation_uni;
		
	moment1_var = dot(pos.xyz - cam_pos_uni, light_dir_uni);// / (clip_uni.y - clip_uni.x);
	uv_var = uv_attr;
	
	gl_Position = modelview_projection_matrix_uni * pos;
}