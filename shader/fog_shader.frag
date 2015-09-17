#version 330

uniform sampler2D depth_tex_uni;
uniform sampler2D color_tex_uni;

uniform vec3 cam_pos_uni;

uniform float start_dist_uni;
uniform float end_dist_uni;
uniform float exp_uni;
uniform vec3 color_uni;

in vec2 uv_coord_var;

out vec4 color_out;



layout(std140) uniform PositionRestoreDataBlock
{
	mat4 modelview_projection_matrix_inv;
	vec2 projection_params;	
} position_restore_data_uni;

vec3 CalculateWorldPosition(void)
{
	float depth = texture(depth_tex_uni, uv_coord_var).x;
	
	vec3 ndc_pos;
	ndc_pos.xy = 2.0 * uv_coord_var - vec2(1.0);
	ndc_pos.z = 2.0 * depth - 1.0;
 
	vec4 clip_pos;
	clip_pos.w = position_restore_data_uni.projection_params.x / (ndc_pos.z - position_restore_data_uni.projection_params.y);
	clip_pos.xyz = ndc_pos * clip_pos.w;
 
	return (position_restore_data_uni.modelview_projection_matrix_inv * clip_pos).xyz;
}


void main(void)
{
	vec4 src_color = texture(color_tex_uni, uv_coord_var);
	vec3 color = src_color.rgb;
	
	vec3 pos = CalculateWorldPosition();
	
	vec3 dir = pos - cam_pos_uni;
	float dist = length(dir);//dir.x*dir.x + dir.y*dir.y + dir.z*dir.z;
	float att = clamp((dist - start_dist_uni) / end_dist_uni, 0.0, 1.0);
	att = pow(att, exp_uni) * src_color.a;
	color = color_uni * att + (1.0 - att) * color; 
	
	color_out = vec4(color, src_color.a);
}