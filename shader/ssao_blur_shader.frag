#version 330

uniform sampler2D tex_uni;

uniform bool blur_vertical_uni; // false => horizontal

in vec2 uv_coord_var;

out vec4 color_out;


void main(void)
{
	vec2 texel_size = 1.0 / textureSize(tex_uni, 0);
	float result = 0.0;
	
	vec2 offset;
	
	if(!blur_vertical_uni) // horizontal
		offset = vec2(2.0, 0.0) * texel_size.x;
	else // vertical
		offset = vec2(0.0, 2.0) * texel_size.y;
		
	result += texture(tex_uni, uv_coord_var + offset * 2).r	* 0.06136;
	result += texture(tex_uni, uv_coord_var + offset).r 	* 0.24477;
	result += texture(tex_uni, uv_coord_var).r 				* 0.38774;
	result += texture(tex_uni, uv_coord_var - offset).r 	* 0.24477;
	result += texture(tex_uni, uv_coord_var - offset * 2).r	* 0.06136;
	
	color_out = vec4(result, 0.0, 0.0, 1.0);
}