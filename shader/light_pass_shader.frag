#version 330

#define MAX_POINT_LIGHTS 8
#define MAX_DIRECTIONAL_LIGHTS 8
#define MAX_DIRECTIONAL_SHADOW_SPLITS 8

#define DIRECTIONAL_LIGHT_SHADOW_LAYER_BLEND_DIST 0.5

uniform vec3 cam_pos_uni;

uniform int point_light_count_uni;
uniform vec3 point_light_pos_uni[MAX_POINT_LIGHTS];
uniform vec3 point_light_color_uni[MAX_POINT_LIGHTS];
uniform float point_light_distance_uni[MAX_POINT_LIGHTS];
uniform bool point_light_shadow_enabled_uni[MAX_POINT_LIGHTS];
uniform samplerCube point_light_shadow_map_uni[MAX_POINT_LIGHTS];

uniform int directional_light_count_uni;
uniform vec3 directional_light_dir_uni[MAX_DIRECTIONAL_LIGHTS];
uniform vec3 directional_light_color_uni[MAX_DIRECTIONAL_LIGHTS];
uniform bool directional_light_shadow_enabled_uni[MAX_DIRECTIONAL_LIGHTS];
uniform vec2 directional_light_shadow_clip_uni[MAX_DIRECTIONAL_LIGHTS];
uniform mat4 directional_light_shadow_tex_matrix_uni[MAX_DIRECTIONAL_LIGHTS * MAX_DIRECTIONAL_SHADOW_SPLITS];
uniform float directional_light_shadow_splits_count_uni[MAX_DIRECTIONAL_LIGHTS];
uniform float directional_light_shadow_splits_z_uni[MAX_DIRECTIONAL_LIGHTS * (MAX_DIRECTIONAL_SHADOW_SPLITS+1)];
uniform sampler2DArray directional_light_shadow_map_uni[MAX_DIRECTIONAL_LIGHTS];

uniform vec3 light_ambient_color_uni;

uniform sampler2D position_tex_uni;
uniform sampler2D diffuse_tex_uni;
uniform sampler2D normal_tex_uni;
uniform sampler2D specular_tex_uni;
uniform sampler2D specular_exponent_tex_uni;

in vec2 uv_coord_var;

out vec4 gl_FragColor;


float linstep(float min, float max, float v);
vec4 PointLightShadowLookup(int i, vec3 dir);
vec4 DirectionalLightShadowLookup(int i, vec3 coord);

void main(void)
{
	vec4 diffuse = texture2D(diffuse_tex_uni, uv_coord_var).rgba;

	if(diffuse.a == 0.0)
		discard;
		
	vec3 position = texture2D(position_tex_uni, uv_coord_var).rgb; 	
	vec3 normal = texture2D(normal_tex_uni, uv_coord_var).rgb;
	vec4 specular = vec4(texture2D(specular_tex_uni, uv_coord_var).rgb, texture2D(specular_exponent_tex_uni, uv_coord_var).r);
	
	
	
	
	
	
	vec3 cam_dir = normalize(cam_pos_uni - position.xyz);
	
	vec3 color = light_ambient_color_uni * diffuse.rgb; // ambient
	float alpha = 1.0;
	
	float light_intensity;
	float shadow = 1.0;
	vec3 specular_color;
	float specular_intensity;
	int i;
	float bias;
	float shadow_depth, light_dist_sq;
	
	for(i=0; i<point_light_count_uni; i++)
	{
		vec3 light_dir = point_light_pos_uni[i] - position.xyz; // pos to light
		light_dist_sq = light_dir.x * light_dir.x + light_dir.y * light_dir.y + light_dir.z * light_dir.z; // squared distance
		if(light_dist_sq > point_light_distance_uni[i] * point_light_distance_uni[i])
			continue;
		float light_dist = sqrt(light_dist_sq); // real distance
		light_dir /= light_dist; // normalized dir
		
		if(point_light_shadow_enabled_uni[i])
		{ 
			vec2 moments = PointLightShadowLookup(i, -light_dir).rg;
			
			float light_depth = length(point_light_pos_uni[i] - position.xyz);// / point_light_distance_uni[i];//light_dist / point_light_distance_uni[i];
									
			// Surface is fully lit. as the current fragment is before the light occluder
			if(light_depth <= moments.x)
				shadow = 1.0;
			else
			{
				float p = smoothstep(light_depth-0.00005, light_depth, moments.x);
			    float variance = max(moments.y - moments.x*moments.x, -0.001);
			    float d = light_depth - moments.x;
			    float p_max = linstep(0.05, 1.0, variance / (variance + d*d));
			    
			    shadow = p_max;//clamp(max(p, p_max), 0.0, 1.0);
			}
		}
		else
			shadow = 1.0;
	
		light_intensity = max(dot(normal, light_dir), 0.0) *  (1.0 - light_dist / point_light_distance_uni[i]);
		color += shadow * light_intensity * point_light_color_uni[i] * diffuse.rgb; // diffuse light
	
		//specular
		specular_color = specular.rgb * point_light_color_uni[i];
		specular_intensity = max(dot(normalize(reflect(-light_dir, normal)), cam_dir), 0.0);// * light_intensity;
		color += max(vec3(0.0, 0.0, 0.0), specular_color * pow(specular_intensity, specular.a)) * shadow;
	}
	
	for(i=0; i<directional_light_count_uni; i++)
	{		
		if(directional_light_shadow_enabled_uni[i])
		{ 
			vec2 coord = vec2(0.0, 0.0);
			int layer = -1;
			
			for(int s=0; s<directional_light_shadow_splits_count_uni[i]; s++)
			{
				coord = ((directional_light_shadow_tex_matrix_uni[i*MAX_DIRECTIONAL_SHADOW_SPLITS + s] * vec4(position.xyz, 1.0)).xy * 0.5) + vec2(0.5, 0.5);
				
				if(!(coord.x < 0.0 || coord.x > 1.0 || coord.y < 0.0 || coord.y > 1.0))
				{	
					layer = s;				
					break;
				}
			}
			
			if(layer != -1)
			{
				vec2 moments = DirectionalLightShadowLookup(i, vec3(coord, float(layer))).rg;
											
				float light_dot = dot(position.xyz - cam_pos_uni, -directional_light_dir_uni[i]) / (directional_light_shadow_clip_uni[i].y - directional_light_shadow_clip_uni[i].x);

				if(light_dot <= moments.x)
					shadow = 1.0;
				else
				{
					float p = smoothstep(light_dot-0.0005, light_dot, moments.x);
				    float variance = max(moments.y - moments.x*moments.x, -0.001);
				    float d = light_dot - moments.x;
				    float p_max = linstep(0.5, 1.0, variance / (variance + d*d));
				    
				   	shadow = clamp(max(p, p_max), 0.0, 1.0);
				}	
			}
			else
				shadow = 1.0;
		}
		else
			shadow = 1.0;	
		
		light_intensity = max(dot(normal, directional_light_dir_uni[i]), 0.0);
		color += shadow * light_intensity * directional_light_color_uni[i] * diffuse.rgb; // diffuse light
	
		//specular
		specular_color = specular.rgb * directional_light_color_uni[i];
		specular_intensity = max(dot(normalize(reflect(-directional_light_dir_uni[i], normal)), cam_dir), 0.0);
		color += max(vec3(0.0, 0.0, 0.0), specular_color * pow(specular_intensity, specular.a)) * shadow;
	}
	
	gl_FragColor = vec4(color, alpha);
}


float linstep(float min, float max, float v)
{
	return clamp((v - min) / (max - min), 0.0, 1.0);
}

vec4 PointLightShadowLookup(int i, vec3 dir)
{
	switch(i)
	{
		case 0: return texture(point_light_shadow_map_uni[0], dir);
		case 1: return texture(point_light_shadow_map_uni[1], dir);
		case 2: return texture(point_light_shadow_map_uni[2], dir);
		case 3: return texture(point_light_shadow_map_uni[3], dir);
		case 4: return texture(point_light_shadow_map_uni[4], dir);
		case 5: return texture(point_light_shadow_map_uni[5], dir);
		case 6: return texture(point_light_shadow_map_uni[6], dir);
		case 7: return texture(point_light_shadow_map_uni[7], dir);
	}
	
	return vec4(1.0);
}

vec4 DirectionalLightShadowLookup(int i, vec3 coord)
{
	switch(i)
	{
		case 0: return texture(directional_light_shadow_map_uni[0], coord);
		case 1: return texture(directional_light_shadow_map_uni[1], coord);
		case 2: return texture(directional_light_shadow_map_uni[2], coord);
		case 3: return texture(directional_light_shadow_map_uni[3], coord);
		case 4: return texture(directional_light_shadow_map_uni[4], coord);
		case 5: return texture(directional_light_shadow_map_uni[5], coord);
		case 6: return texture(directional_light_shadow_map_uni[6], coord);
		case 7: return texture(directional_light_shadow_map_uni[7], coord);
	}
	
	return vec4(1.0);
}
