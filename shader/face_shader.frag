#version 130

// -------------------------------------

//in vec4 TexBorder;
//in float DiscardBorder;

//in float BumpFactor;

// -------------------------------------

#define M_PI 3.1415926535897932384626433832795

#define MAX_POINT_LIGHTS 8
#define MAX_DIRECTIONAL_LIGHTS 8

uniform bool shader_mode_uni;

uniform bool two_side_uni;

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
uniform bool point_light_shadow_enabled_uni[MAX_POINT_LIGHTS];
uniform samplerCube point_light_shadow_map_uni[MAX_POINT_LIGHTS];
uniform float point_light_shadow_near_clip_uni[MAX_POINT_LIGHTS];

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
//in vec4 shadow_coord_var;

out vec4 gl_FragColor;

float VectorToDepth(vec3 Vec, float n, float f)
{
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    float NormZComp = (f+n) / (f-n) - (2*f*n)/(f-n)/LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}

float PointLightShadowLookup(int i, vec3 dir)
{
	switch(i)
	{
		case 0: return texture(point_light_shadow_map_uni[0], dir).r;
		case 1: return texture(point_light_shadow_map_uni[1], dir).r;
		case 2: return texture(point_light_shadow_map_uni[2], dir).r;
		case 3: return texture(point_light_shadow_map_uni[3], dir).r;
		case 4: return texture(point_light_shadow_map_uni[4], dir).r;
		case 5: return texture(point_light_shadow_map_uni[5], dir).r;
		case 6: return texture(point_light_shadow_map_uni[6], dir).r;
		case 7: return texture(point_light_shadow_map_uni[7], dir).r;
	}
	
	return 1.0;
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
	float shadow = 1.0;
	vec3 specular_color;
	float specular_intensity;
	int i;
	float bias;
	float shadow_depth, light_depth;
		
	for(i=0; i<point_light_count_uni; i++)
	{
		vec3 light_dir = point_light_pos_uni[i] - pos_var; // pos to light
		float light_dist = light_dir.x * light_dir.x + light_dir.y * light_dir.y + light_dir.z * light_dir.z; // squared distance
		if(light_dist > point_light_distance_uni[i] * point_light_distance_uni[i])
			continue;
		light_dist = sqrt(light_dist); // real distance
		light_dir /= light_dist; // normalized dir
		
		
		if(point_light_shadow_enabled_uni[i])
		{
			int x, y;
			vec3 shadow_dir = -light_dir;
			vec2 shadow_dir_xz = normalize(shadow_dir.xz);
			vec2 shadow_rot;
			vec2 shadow_rot_v;
			float sr = 0.002;
			vec3 pcf_dir;
			
			shadow_rot.x = asin(shadow_dir.y);
			shadow_rot.y = asin(shadow_dir_xz.y);
			if(shadow_dir.x < 0.0)
				shadow_rot.y = M_PI - shadow_rot.y;
				
			shadow = 0.0;
			
			int samples = 2;
			
			for (y=-samples; y<=samples; y++)
					for (x=-samples; x<=samples; x++)
					{
						shadow_rot_v = vec2(shadow_rot.x + float(x) * sr, shadow_rot.y + float(y) * sr);
						pcf_dir.y = sin(shadow_rot_v.x);
						pcf_dir.z = sin(shadow_rot_v.y) * cos(shadow_rot_v.x);
						pcf_dir.x = cos(shadow_rot_v.y) * cos(shadow_rot_v.x); 
						shadow_depth = PointLightShadowLookup(i, pcf_dir);
						//bias = 0.0005 * tan(acos(clamp(dot(normal_var, light_dir), 0.0, 1.0)));
						//bias = clamp(bias, 0.0, 0.0015);
						bias = 0.001 + abs(sqrt(float(x * x  + y * y))) * 0.0005 * tan(acos(clamp(dot(normal_var, light_dir), 0.0, 1.0)));
						light_depth = VectorToDepth(light_dir * (light_dist - bias), point_light_shadow_near_clip_uni[i], point_light_distance_uni[i]);
						if(shadow_depth + bias >= light_depth)
							shadow += 1.0;
					}
			shadow /= (samples * 2 + 1) * (samples * 2 + 1);
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














