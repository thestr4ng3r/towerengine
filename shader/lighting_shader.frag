#version 330

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shading_language_include : require

#include "position_restore.glsl"

uniform vec3 cam_pos_uni;

uniform sampler2D depth_tex_uni;
uniform sampler2D diffuse_tex_uni;
uniform sampler2D normal_tex_uni;
uniform sampler2D specular_tex_uni;
uniform sampler2D self_illumination_tex_uni;

uniform bool ssao_enabled_uni;
uniform sampler2D ssao_tex_uni;

in vec2 uv_coord_var;

out vec4 color_out;

// ambient

uniform vec3 light_ambient_color_uni;

// point lights

#pragma define_param(MAX_POINT_LIGHTS_COUNT max_point_lights_count)

struct PointLight
{
	float dist;
	int shadow_enabled;
	vec3 pos;
	vec3 color;
	samplerCube shadow_map;
};


layout(std140) uniform PointLightBlock
{	
	int count;
	PointLight light[MAX_POINT_LIGHTS_COUNT];
} point_light_uni;





float linstep(float min, float max, float v)
{
	return clamp((v - min) / (max - min), 0.0, 1.0);
}

void main(void)
{	
	float depth = texture(depth_tex_uni, uv_coord_var).x;
	
	if(depth == 1.0)
		discard;

	vec3 diffuse = texture(diffuse_tex_uni, uv_coord_var).rgb;	
	vec3 position = CalculateWorldPosition(depth, uv_coord_var);
	vec3 normal = normalize(texture(normal_tex_uni, uv_coord_var).rgb * 2.0 - vec3(1.0, 1.0, 1.0));
	vec4 specular = texture(specular_tex_uni, uv_coord_var).rgba;
		
	vec3 cam_dir = normalize(cam_pos_uni - position.xyz);
	
	
	
	// ambient lighting, ssao and self illumination
	
	vec3 color = diffuse.rgb * light_ambient_color_uni;
	
	if(ssao_enabled_uni)
	{
		float occlusion = texture(ssao_tex_uni, uv_coord_var).r;
		color *= occlusion;
	}
	
	vec3 self_illumination = texture(self_illumination_tex_uni, uv_coord_var).rgb;
	color += self_illumination;
	
	
	// point lighting
	
	float shadow;
	vec3 light_dir;
	float light_dist_sq;
	float light_dist;
	float light_dist_attenuation;
	float light_intensity;
	vec3 specular_color;
	float specular_intensity;
	
	for(int i=0; i<point_light_uni.count; i++)
	{
		shadow = 1.0;
	
		light_dir = point_light_uni.light[i].pos - position.xyz; // pos to light
		light_dist_sq = light_dir.x * light_dir.x + light_dir.y * light_dir.y + light_dir.z * light_dir.z; // squared distance
		if(light_dist_sq <= point_light_uni.light[i].dist * point_light_uni.light[i].dist)
		{
			light_dist = sqrt(light_dist_sq); // real distance
			light_dir /= light_dist; // normalized dir
			
			if(point_light_uni.light[i].shadow_enabled != 0)
			{ 
				vec2 moments = texture(point_light_uni.light[i].shadow_map, -light_dir).rg;
				//vec2 moments = vec2(0.0);
				
				float light_depth = length(point_light_uni.light[i].pos - position.xyz) - 0.01;
							
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
		
		
			light_dist_attenuation = (1.0 - light_dist / point_light_uni.light[i].dist);
			light_intensity = max(dot(normal, light_dir), 0.0) * light_dist_attenuation;
			color += shadow * light_intensity * point_light_uni.light[i].color * diffuse.rgb; // diffuse light
		
			//specular
			specular_color = specular.rgb * point_light_uni.light[i].color;
			specular_intensity = max(dot(normalize(reflect(-light_dir, normal)), cam_dir), 0.0);
			color += max(vec3(0.0, 0.0, 0.0), specular_color * pow(specular_intensity, specular.a)) * shadow * light_dist_attenuation;
		}
	}
	
	color_out = vec4(color, 1.0);
}