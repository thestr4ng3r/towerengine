#version 330

#extension GL_ARB_shading_language_include : require

#include "position_restore.glsl"

uniform sampler2D depth_tex_uni;
uniform sampler2D color_tex_uni;

uniform vec3 cam_pos_uni;

uniform float start_dist_uni;
uniform float end_dist_uni;
uniform float exp_uni;
uniform vec3 color_uni;

in vec2 uv_coord_var;

out vec4 color_out;




void main(void)
{
	float depth = texture(depth_tex_uni, uv_coord_var).x;
	if(depth == 1.0)
		discard;
	
	vec4 src_color = texture(color_tex_uni, uv_coord_var);
	vec3 color = src_color.rgb;
	
	vec3 pos = CalculateWorldPosition(depth, uv_coord_var);
	
	vec3 dir = pos - cam_pos_uni;
	float dist = length(dir);//dir.x*dir.x + dir.y*dir.y + dir.z*dir.z;
	float att = clamp((dist - start_dist_uni) / end_dist_uni, 0.0, 1.0);
	att = pow(att, exp_uni) * src_color.a;
	color = color_uni * att + (1.0 - att) * color; 
	
	color_out = vec4(color, src_color.a);
}
