#version 330

#define M_PI 3.1415926535897932384626433832795

uniform vec3 diffuse_color_uni;
uniform vec4 diffuse_color2_uni;
uniform vec3 specular_color_uni;
uniform float specular_size_uni;
uniform float bump_depth_uni;

uniform bool diffuse_tex_enabled_uni;
uniform bool specular_tex_enabled_uni;
uniform bool normal_tex_enabled_uni;
uniform bool bump_tex_enabled_uni;

uniform sampler2D diffuse_tex_uni;
uniform sampler2D normal_tex_uni;
uniform sampler2D specular_tex_uni;
uniform sampler2D bump_tex_uni;


uniform vec3 clip_vec_uni;
uniform float clip_dist_uni;


in vec3 pos_var;
in vec3 normal_var;
in vec3 tang_var;
in vec3 bitang_var;
in vec2 uv_var;

in vec3 cam_dir_var;


layout (location = 0) out vec4 position_out;
layout (location = 1) out vec4 diffuse_out;
layout (location = 2) out vec4 normal_out;
layout (location = 3) out vec4 specular_out; 
layout (location = 4) out vec4 specular_exponent_out;



void main(void)
{
	if(!gl_FrontFacing) // backface culling
		discard;
		
	if(clip_vec_uni != vec3(0.0, 0.0, 0.0)) // face clipping for water
	{
		vec3 clip = pos_var - clip_vec_uni * clip_dist_uni;
		if(dot(clip, clip_vec_uni) >= 0.0)
			discard;
	}
	
	
	mat3 tang_mat = mat3(tang_var, bitang_var, normal_var);
	
	vec2 uv = uv_var;
	
	if(bump_tex_enabled_uni)
	{
		float height = texture2D(bump_tex_uni, uv_var).r * bump_depth_uni;
		vec3 cam_offset = height * normalize(cam_dir_var);
		vec2 uv_offset;
		uv_offset.x = dot(cam_offset, tang_var);
		uv_offset.y = dot(cam_offset, bitang_var);
		uv += uv_offset;
	}

				
	vec4 diffuse_color = vec4(1.0, 1.0, 1.0, 1.0);
	
	if(diffuse_tex_enabled_uni)
		diffuse_color = texture2D(diffuse_tex_uni, uv).rgba;
		
	diffuse_color *= vec4(diffuse_color_uni.rgb, 1.0) * diffuse_color2_uni;
	
	if(normal_tex_enabled_uni)
	{
		vec3 normal_tex_color = texture2D(normal_tex_uni, uv).rgb;
		normal_out = vec4(tang_mat * ((normal_tex_color - vec3(0.5, 0.5, 0.5)) * 2.0), 1.0);
	}
	else
		normal_out = vec4(normal_var, 1.0);
			
	vec3 specular_color = specular_color_uni;
	if(specular_tex_enabled_uni)
		specular_color *= texture2D(specular_tex_uni, uv).rgb;
	
	position_out = vec4(pos_var, 1.0);
	diffuse_out = diffuse_color;
	specular_out = vec4(specular_color, 1.0);
	specular_exponent_out = vec4(specular_size_uni, 0.0, 0.0, 1.0);
}
















