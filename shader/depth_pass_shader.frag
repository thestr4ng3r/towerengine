#version 330

uniform bool diffuse_tex_enabled_uni;
uniform sampler2D diffuse_tex_uni;

//uniform vec3 clip_vec_uni;
//uniform float clip_dist_uni;

in vec2 uv_var;


void main(void)
{
	//if(!gl_FrontFacing) // backface culling
	//	discard;

	/*if(clip_vec_uni != vec3(0.0, 0.0, 0.0)) // face clipping for water
	{
		vec3 clip = pos_var - clip_vec_uni * clip_dist_uni;
		if(dot(clip, clip_vec_uni) >= 0.0)
			discard;
	}*/

	
	if(diffuse_tex_enabled_uni)
	{
		if(texture(diffuse_tex_uni, uv_var).a < 0.5)
			discard;
	}
}