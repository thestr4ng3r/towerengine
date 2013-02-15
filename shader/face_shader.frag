#version 130

// -------------------------------------

//in vec4 TexBorder;
//in float DiscardBorder;

//in float BumpFactor;

// -------------------------------------

uniform bool shader_mode_uni;

uniform bool two_side_uni;

uniform sampler2DShadow shadow_map_uni;
uniform bool shadow_enabled_uni;
uniform vec2 shadow_pixel_size_uni;

uniform vec3 diffuse_color_uni;
uniform vec4 diffuse_color2_uni;
uniform vec3 specular_color_uni;
uniform float ambient_uni;
uniform float specular_size_uni;

uniform bool diffuse_tex_enabled_uni;
uniform bool specular_tex_enabled_uni;
uniform bool normal_tex_enabled_uni;

uniform sampler2D diffuse_tex_uni;
uniform sampler2D normal_tex_uni;
uniform sampler2D specular_tex_uni;

uniform vec3 light_pos_uni;
uniform vec3 light_color_uni;
uniform vec3 light_ambient_color_uni;

uniform vec3 clip_vec_uni;
uniform float clip_dist_uni;


in vec3 pos_var;
in vec3 normal_var;
in vec3 tang_var;
in vec3 bitang_var;
in vec2 uv_var;

in vec3 cam_pos_var;
in vec4 shadow_coord_var;


out vec4 gl_FragColor;


vec4 ShadowCoordO(vec2 offset)
{
	return shadow_coord_var + vec4(offset.x * shadow_pixel_size_uni.x * shadow_coord_var.w, offset.y * shadow_pixel_size_uni.y * shadow_coord_var.w, -0.0005, 0.0);
}

vec4 ShadowCoordW(vec2 offset)
{
	return ShadowCoordO(offset) / shadow_coord_var.w;
}

float ShadowLookup(vec2 offset)
{
	return textureProj(shadow_map_uni, ShadowCoordO(offset));
}


void main(void)
{
	if(!gl_FrontFacing && !two_side_uni) // backface culling
		discard;
		
	if(clip_vec_uni != vec3(0.0, 0.0, 0.0)) // face clipping for water
	{
		vec3 clip = pos_var - clip_vec_uni * clip_dist_uni;
		if(dot(clip, clip_vec_uni) >= 0.0)
			discard;
	}
	
	if(!shader_mode_uni) // shader mode
	{
		gl_FragColor = vec4(1.0);
		return;
	}
	
	vec3 light_dir = normalize(light_pos_uni - pos_var); // pos to light normalized
	vec3 cam_dir = normalize(cam_pos_var - pos_var); // pos to cam normalized
	mat3 tangent_space = mat3(tang_var, bitang_var, normal_var);
				
	vec4 diffuse_tex_color = vec4(1.0, 1.0, 1.0, 1.0);
	if(diffuse_tex_enabled_uni)
		diffuse_tex_color = texture2D(diffuse_tex_uni, uv_var).rgba;
	
	vec3 normal_tex_color = vec3(0.5, 0.5, 1.0);
	if(normal_tex_enabled_uni)
		normal_tex_color = texture2D(normal_tex_uni, uv_var).rgb;
	vec3 specular_tex_color = vec3(0.5, 0.5, 0.5);
	if(specular_tex_enabled_uni)
		specular_tex_color = texture2D(specular_tex_uni, uv_var).rgb;
	
	vec3 normal = tangent_space * (normal_tex_color - vec3(0.5, 0.5, 0.5)) * 2.0; 
	
	float alpha = diffuse_tex_color.a; // alpha
	vec3 color = light_ambient_color_uni * diffuse_tex_color.rgb * ambient_uni; // ambient
	
	float shadow = 1.0;
	float sr = 1.5;
	if(	shadow_enabled_uni &&
		!(ShadowCoordW(vec2(sr, 0.0)).x > 1.0 || ShadowCoordW(vec2(0.0, sr)).y > 1.0 ||	ShadowCoordW(vec2(sr, 0.0)).x < 0.0 || ShadowCoordW(vec2(0.0, sr)).y < 0.0) &&
		!(shadow_coord_var.z / shadow_coord_var.w > 1.0))
			
	{
		float x,y;
		shadow = 0.0;
		for (y=-sr; y<=sr; y+=(sr*2.0 + 1.0)/4.0)
			for (x = -sr ; x <=sr ; x+=(sr*2.0 + 1.0)/4.0)
				shadow += ShadowLookup(vec2(x,y));
		shadow /= 16.0;
	}
	
	
	float light_intensity = dot(normal, light_dir);
	color += shadow * light_intensity * light_color_uni * diffuse_tex_color.rgb; // diffuse light
	
	//specular
	vec3 specular_color = specular_tex_color * specular_color_uni * texture2D(specular_tex_uni, uv_var).rgb * light_color_uni;
	float specular_intensity = max(dot(normalize(reflect(-light_dir, normal)), cam_dir), 0.0);
	color += max(vec3(0.0, 0.0, 0.0), specular_color * pow(specular_intensity, specular_size_uni)) * light_intensity;
		
	gl_FragColor = vec4(color, alpha) * diffuse_color2_uni;
}














