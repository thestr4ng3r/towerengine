#version 330

#extension GL_ARB_shading_language_include : require

#include "position_restore.glsl"

layout(std140) uniform MatrixBlock
{
	mat4 modelview_projection_matrix;
} matrix_uni;


uniform vec3 color_uni;
uniform vec4 edge_color_uni;

uniform bool color_tex_enabled_uni;
uniform sampler2D color_tex_uni;

uniform bool normal_tex_enabled_uni;
uniform sampler2D normal_tex_uni;

uniform sampler2D screen_tex_uni;

in vec3 pos_var;
in vec3 normal_var;
in vec3 tang_var;
in vec3 bitang_var;

in vec2 uv_var;
in vec3 cam_dir_var;

out vec4 color_out;

void main(void)
{
	if(!gl_FrontFacing)
		discard;
	
	vec4 color = vec4(color_uni, 1.0);
	
	if(color_tex_enabled_uni)
		color *= texture(color_tex_uni, uv_var).rgba;
		
	mat3 tang_mat = mat3(normalize(tang_var), normalize(bitang_var), normalize(normal_var));
		
	vec3 normal;
	if(normal_tex_enabled_uni)
		normal = (tang_mat * (texture(normal_tex_uni, uv_var).xyz * 2.0 - vec3(1.0))).xyz;
	else
		normal = normal_var;
	normal = normalize(normal);
	
	vec3 ref = refract(normalize(cam_dir_var), normal, 0.2);
	
	vec4 screen_pos = matrix_uni.modelview_projection_matrix * vec4(pos_var + ref * 0.02, 1.0);
	vec2 screen_uv = (screen_pos.xy / screen_pos.w) * vec2(1.0 / position_restore_data_uni.uv_factor_x, 0.5) + vec2(-position_restore_data_uni.uv_offset_x, 0.5);
	color.rgb *= texture(screen_tex_uni, screen_uv).rgb;	

	if(edge_color_uni.a > 0.0)
	{
		float edge_blend = clamp(dot(normalize(-cam_dir_var), normal), 0.0, 1.0);
		color.rgb = edge_color_uni.a * (edge_blend * color.rgb + (1.0 - edge_blend) * edge_color_uni.rgb)
					+ (1.0 - edge_color_uni.a) * color.rgb;
		
	}
		
	color_out = color;
}
