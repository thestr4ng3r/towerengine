#version 330

uniform bool diffuse_tex_enabled_uni;
uniform sampler2D diffuse_tex_uni;

in vec3 pos_var;
in vec2 uv_var;

uniform vec3 light_pos_uni;
uniform float light_dist_uni;

out vec4 color_out;

void main(void)
{
	vec3 dir = pos_var - light_pos_uni;
	float dist_sq = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
	
	if(dist_sq > light_dist_uni * light_dist_uni)
		discard;


	if(diffuse_tex_enabled_uni)
	{
		if(texture(diffuse_tex_uni, uv_var).a < 0.5)
			discard;
	}
	
	float dist = sqrt(dist_sq);
	
	color_out = vec4(dist, dist_sq, 0.0, 1.0);
}














