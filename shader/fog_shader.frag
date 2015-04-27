#version 330

uniform sampler2D position_tex_uni;
uniform sampler2D color_tex_uni;

uniform vec3 cam_pos_uni;

uniform float start_dist_uni;
uniform float end_dist_uni;
uniform float exp_uni;
uniform vec3 color_uni;

in vec2 uv_coord_var;

out vec4 gl_FragColor;


void main(void)
{
	vec4 src_color = texture(color_tex_uni, uv_coord_var);
	vec3 color = src_color.rgb;
	
	vec3 pos = texture(position_tex_uni, uv_coord_var).xyz;
	
	vec3 dir = pos - cam_pos_uni;
	float dist = length(dir);//dir.x*dir.x + dir.y*dir.y + dir.z*dir.z;
	float att = clamp((dist - start_dist_uni) / end_dist_uni, 0.0, 1.0);
	att = pow(att, exp_uni) * src_color.a;
	color = color_uni * att + (1.0 - att) * color; 
	
	gl_FragColor = vec4(color, src_color.a);
}