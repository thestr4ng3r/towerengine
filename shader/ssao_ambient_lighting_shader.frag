#version 330

uniform vec3 light_ambient_color_uni;

uniform sampler2D depth_tex_uni;
uniform sampler2D ssao_tex_uni;
uniform sampler2D diffuse_tex_uni;
uniform sampler2D self_illumination_tex_uni;

in vec2 uv_coord_var;

out vec4 color_out;

void main(void)
{
	float depth = texture(depth_tex_uni, uv_coord_var).x;
	if(depth == 1.0)
		discard;
		
	vec3 diffuse = texture(diffuse_tex_uni, uv_coord_var).rgb;	
	float occlusion = texture(ssao_tex_uni, uv_coord_var).r;
	vec3 self_illumination = texture(self_illumination_tex_uni, uv_coord_var).rgb;
		
	vec3 color = light_ambient_color_uni * diffuse.rgb * occlusion + self_illumination;
	
	color_out = vec4(color, 1.0);
}