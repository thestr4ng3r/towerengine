#version 130

// -------------------------------------

//in vec4 TexBorder;
//in float DiscardBorder;

//in float BumpFactor;

// -------------------------------------

#define MAX_POINT_LIGHTS 16
#define MAX_DIRECTIONAL_LIGHTS 16

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

uniform int point_light_count_uni;
uniform vec3 point_light_pos_uni[MAX_POINT_LIGHTS];
uniform vec3 point_light_color_uni[MAX_POINT_LIGHTS];
uniform float point_light_distance_uni[MAX_POINT_LIGHTS];

uniform int directional_light_count_uni;
uniform vec3 directional_light_dir_uni[MAX_DIRECTIONAL_LIGHTS];
uniform vec3 directional_light_color_uni[MAX_DIRECTIONAL_LIGHTS];

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
	return shadow_coord_var + vec4(offset.x * shadow_pixel_size_uni.x * shadow_coord_var.w, offset.y * shadow_pixel_size_uni.y * shadow_coord_var.w, -0.002, 0.0);
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
	

	vec3 cam_dir = normalize(cam_pos_var - pos_var); // pos to cam normalized
	mat3 tangent_space = mat3(tang_var, bitang_var, normal_var);
				
	vec4 diffuse_color = vec4(1.0, 1.0, 1.0, 1.0);
	if(diffuse_tex_enabled_uni)
		diffuse_color = texture2D(diffuse_tex_uni, uv_var).rgba;
	diffuse_color *= vec4(diffuse_color_uni.rgb, 1.0);
	
	vec3 normal_tex_color = vec3(0.5, 0.5, 1.0);
	if(normal_tex_enabled_uni)
		normal_tex_color = texture2D(normal_tex_uni, uv_var).rgb;
	vec3 specular_tex_color = vec3(0.5, 0.5, 0.5);
	if(specular_tex_enabled_uni)
		specular_tex_color = texture2D(specular_tex_uni, uv_var).rgb;
	
	vec3 normal = tangent_space * (normal_tex_color - vec3(0.5, 0.5, 0.5)) * 2.0; 
	
	float alpha = diffuse_color.a; // alpha
	vec3 color = light_ambient_color_uni * diffuse_color.rgb * ambient_uni; // ambient
	
	float light_intensity;
	float shadow;
	vec3 specular_color;
	float specular_intensity;
	int i;
	
	for(i=0; i<point_light_count_uni; i++)
	{
		vec3 light_dir = point_light_pos_uni[i] - pos_var; // pos to light
		float light_dist = light_dir.x * light_dir.x + light_dir.y * light_dir.y + light_dir.z * light_dir.z; // squared distance
		if(light_dist > point_light_distance_uni[i] * point_light_distance_uni[i])
			continue;
		light_dist = sqrt(light_dist); // real distance
		light_dir /= light_dist; // normalized dir
		
		shadow = 1.0;
		if(i==0) // shadow only for first point light at the moment
		{
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
		}
	
	
		light_intensity = max(dot(normal, light_dir), 0.0) *  (1.0 - light_dist / point_light_distance_uni[i]);
		color += shadow * light_intensity * point_light_color_uni[i] * diffuse_color.rgb; // diffuse light
	
		//specular
		specular_color = specular_tex_color * specular_color_uni * point_light_color_uni[i];
		specular_intensity = max(dot(normalize(reflect(-light_dir, normal)), cam_dir), 0.0) * light_intensity;
		color += max(vec3(0.0, 0.0, 0.0), specular_color * pow(specular_intensity, specular_size_uni)) * shadow;
	}
	
	for(i=0; i<directional_light_count_uni; i++)
	{
		shadow = 1.0;
		light_intensity = max(dot(normal, directional_light_dir_uni[i]), 0.0);
		color += shadow * light_intensity * directional_light_color_uni[i] * diffuse_color.rgb; // diffuse light
	
		//specular
		specular_color = specular_tex_color * specular_color_uni * directional_light_color_uni[i];
		specular_intensity = max(dot(normalize(reflect(-directional_light_dir_uni[i], normal)), cam_dir), 0.0) * light_intensity;
		color += max(vec3(0.0, 0.0, 0.0), specular_color * pow(specular_intensity, specular_size_uni)) * shadow;
	}
		
	gl_FragColor = vec4(color, alpha) * diffuse_color2_uni;
}














