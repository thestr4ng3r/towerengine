#version 330

uniform vec3 cam_pos_uni;

uniform vec3 point_light_pos_uni;
uniform vec3 point_light_color_uni;
uniform float point_light_distance_uni;
uniform bool point_light_shadow_enabled_uni;
uniform samplerCube point_light_shadow_map_uni;


uniform sampler2D position_tex_uni;
uniform sampler2D diffuse_tex_uni;
uniform sampler2D normal_tex_uni;
uniform sampler2D specular_tex_uni;
uniform sampler2D specular_exponent_tex_uni;


in vec2 uv_coord_var;

out vec4 gl_FragColor;


float linstep(float min, float max, float v)
{
	return clamp((v - min) / (max - min), 0.0, 1.0);
}

void main(void)
{
	vec4 diffuse = texture2D(diffuse_tex_uni, uv_coord_var).rgba;

	if(diffuse.a == 0.0)
		discard;
		
	vec3 position = texture2D(position_tex_uni, uv_coord_var).rgb; 	
	vec3 normal = normalize(texture2D(normal_tex_uni, uv_coord_var).rgb * 2.0 - vec3(1.0, 1.0, 1.0));
	vec4 specular = vec4(texture2D(specular_tex_uni, uv_coord_var).rgb, texture2D(specular_exponent_tex_uni, uv_coord_var).r);
	
	
	
	vec3 cam_dir = normalize(cam_pos_uni - position.xyz);
	
	float shadow = 1.0;

	vec3 light_dir = point_light_pos_uni - position.xyz; // pos to light
	float light_dist_sq = light_dir.x * light_dir.x + light_dir.y * light_dir.y + light_dir.z * light_dir.z; // squared distance
	if(light_dist_sq > point_light_distance_uni * point_light_distance_uni)
	{
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}
	float light_dist = sqrt(light_dist_sq); // real distance
	light_dir /= light_dist; // normalized dir
	
	if(point_light_shadow_enabled_uni)
	{ 
		vec2 moments = texture(point_light_shadow_map_uni, -light_dir).rg;
		
		float light_depth = length(point_light_pos_uni - position.xyz);
								
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

	float light_dist_attenuation = (1.0 - light_dist / point_light_distance_uni);
	float light_intensity = max(dot(normal, light_dir), 0.0) *  light_dist_attenuation;
	vec3 color = shadow * light_intensity * point_light_color_uni * diffuse.rgb; // diffuse light

	//specular
	vec3 specular_color = specular.rgb * point_light_color_uni;
	float specular_intensity = max(dot(normalize(reflect(-light_dir, normal)), cam_dir), 0.0);
	color += max(vec3(0.0, 0.0, 0.0), specular_color * pow(specular_intensity, specular.a)) * shadow * light_dist_attenuation;
	
	
	gl_FragColor = vec4(color, 1.0);
}