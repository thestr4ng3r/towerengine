#version 450 core

#extension GL_ARB_bindless_texture : require


uniform vec3 cam_pos_uni;

uniform sampler2D position_tex_uni;
uniform sampler2D normal_tex_uni;
uniform sampler2D tang_tex_uni;
uniform sampler2D uv_tex_uni;
uniform usampler2D material_tex_uni;

in vec2 uv_coord_var;

out vec4 color_out;


uniform sampler2D diffuse_tex_uni[128];



// ambient

uniform vec3 light_ambient_color_uni;

// point lights

#define MAX_POINT_LIGHTS_COUNT 16

uniform int point_light_count_uni;
uniform vec3 point_light_pos_uni[MAX_POINT_LIGHTS_COUNT];
uniform vec3 point_light_color_uni[MAX_POINT_LIGHTS_COUNT];
uniform float point_light_distance_uni[MAX_POINT_LIGHTS_COUNT];
uniform bool point_light_shadow_enabled_uni[MAX_POINT_LIGHTS_COUNT];
uniform samplerCube point_light_shadow_map_uni[MAX_POINT_LIGHTS_COUNT];


float linstep(float min, float max, float v)
{
	return clamp((v - min) / (max - min), 0.0, 1.0);
}

void main(void)
{
	ivec2 texel_uv = ivec2(uv_coord_var * textureSize(position_tex_uni, 0).xy);

	vec3 position = texelFetch(position_tex_uni, texel_uv, 0).xyz;
	vec3 normal = texelFetch(normal_tex_uni, texel_uv, 0).xyz;
	vec4 tang_data = texelFetch(tang_tex_uni, texel_uv, 0);
	vec3 tang = tang_data.xyz;
	vec2 uv = texelFetch(uv_tex_uni, texel_uv, 0).xy;
	uint material_index = texelFetch(material_tex_uni, texel_uv, 0).x;
	
	normal = normalize(normal);
	tang = normalize(tang);
	vec3 bitang = cross(normal, tang);
	if(tang_data.w < 0.0)
		bitang *= -1.0;
	bitang = normalize(bitang);
		
	vec3 cam_dir = normalize(cam_pos_uni - position.xyz);
	
	// material data
	
	vec3 diffuse = texture(diffuse_tex_uni[material_index], uv).rgb;
	vec4 specular = vec4(1.0, 1.0, 1.0, 32.0); // TODO
	
	
	
	
	// ambient lighting
	
	vec3 color = diffuse * light_ambient_color_uni;
	
	
	// point lighting
	
	float shadow;
	vec3 light_dir;
	float light_dist_sq;
	float light_dist;
	float light_dist_attenuation;
	float light_intensity;
	vec3 specular_color;
	float specular_intensity;
	
	for(int i=0; i<point_light_count_uni; i++)
	{
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
				//vec2 moments = vec2(0.0);
				
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
	}
	
	
	
	
	
	
	color_out = vec4(color, 1.0);
}