#version 330

#extension GL_ARB_shading_language_include : require

#include "position_restore.glsl"

uniform vec3 cam_pos_uni;

#pragma define_param(LIGHTS_COUNT lights_count)

uniform vec3 point_light_pos_uni[LIGHTS_COUNT];
uniform vec3 point_light_color_uni[LIGHTS_COUNT];
uniform float point_light_distance_uni[LIGHTS_COUNT];
uniform bool point_light_shadow_enabled_uni[LIGHTS_COUNT];
uniform samplerCube point_light_shadow_map_uni[LIGHTS_COUNT];


uniform sampler2D depth_tex_uni;
uniform sampler2D diffuse_tex_uni;
uniform sampler2D normal_tex_uni;
uniform sampler2D specular_tex_uni;


in vec2 uv_coord_var;

out vec4 color_out;






float linstep(float min, float max, float v)
{
	return clamp((v - min) / (max - min), 0.0, 1.0);
}

void main(void)
{
	float depth = texture(depth_tex_uni, uv_coord_var).x;
	if(depth == 1.0)
		discard;
	
	ivec2 texel_uv = ivec2(uv_coord_var * textureSize(diffuse_tex_uni, 0).xy);	
	
	vec3 diffuse = texelFetch(diffuse_tex_uni, texel_uv, 0).rgb;
	vec3 position = CalculateWorldPosition(depth, uv_coord_var);
	vec3 normal = normalize(texelFetch(normal_tex_uni, texel_uv, 0).rgb * 2.0 - vec3(1.0, 1.0, 1.0));
	vec4 specular = texelFetch(specular_tex_uni, texel_uv, 0).rgba;
	
	vec3 cam_dir = normalize(cam_pos_uni - position.xyz);
	
	float shadow;
	vec3 light_dir;
	float light_dist_sq;
	float light_dist;
	float light_dist_attenuation;
	float light_intensity;
	vec3 specular_color;
	float specular_intensity;
	vec3 color = vec3(0.0, 0.0, 0.0);

	int i;
	#pragma for(i from 0 ex param lights_count)
		shadow = 1.0;
	
		light_dir = point_light_pos_uni[i] - position.xyz; // pos to light
		light_dist_sq = light_dir.x * light_dir.x + light_dir.y * light_dir.y + light_dir.z * light_dir.z; // squared distance
		if(light_dist_sq <= point_light_distance_uni[i] * point_light_distance_uni[i])
		{
			light_dist = sqrt(light_dist_sq); // real distance
			light_dir /= light_dist; // normalized dir
			
			if(point_light_shadow_enabled_uni[i])
			{ 
				vec2 moments = texture(point_light_shadow_map_uni[i], -light_dir).rg;
				
				float light_depth = length(point_light_pos_uni[i] - position.xyz) - 0.01;
										
				// Surface is fully lit. as the current fragment is before the light occluder
				if(light_depth <= moments.x)
					shadow = 1.0;
				else
				{
					float p = smoothstep(light_depth-0.00005, light_depth, moments.x);
				    float variance = max(moments.y - moments.x*moments.x, -0.001);
				    float d = light_depth - moments.x;
				    float p_max = linstep(0.3, 1.0, variance / (variance + d*d));
				    
				    shadow = p_max;//clamp(max(p, p_max), 0.0, 1.0);
				}
			}
			else
				shadow = 1.0;
		
		
			light_dist_attenuation = (1.0 - light_dist / point_light_distance_uni[i]);
			light_intensity = max(dot(normal, light_dir), 0.0) *  light_dist_attenuation;
			color += shadow * light_intensity * point_light_color_uni[i] * diffuse.rgb; // diffuse light
		
			//specular
			specular_color = specular.rgb * point_light_color_uni[i];
			specular_intensity = max(dot(normalize(reflect(-light_dir, normal)), cam_dir), 0.0);
			color += max(vec3(0.0, 0.0, 0.0), specular_color * pow(specular_intensity, specular.a)) * shadow * light_dist_attenuation;
		}
	#pragma endfor
	
	color_out = vec4(color, 1.0);
}
