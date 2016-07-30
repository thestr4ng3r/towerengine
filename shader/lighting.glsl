

#define M_PI 3.1415926535897932384626433832795

float linstep(float min, float max, float v)
{
	return clamp((v - min) / (max - min), 0.0, 1.0);
}

float BRDF_SpecularD_GGX(float roughness, float n_dot_h)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float n_dot_h2 = n_dot_h * n_dot_h;
	float d = n_dot_h2 * (alpha2 - 1.0) + 1.0;
	return alpha2 / (M_PI * d * d);
}

float BRDF_SpecularG_Schlick(float roughness, float n_dot_l, float n_dot_v)
{
	float k = ((roughness + 1.0) * (roughness + 1.0)) / 8.0;

	float G_l = n_dot_l / (n_dot_l * (1.0 - k) + k);
	float G_v = n_dot_v / (n_dot_v * (1.0 - k) + k);

	return G_l * G_v;
}

float BRDF_SpecularF_Schlick(float f0, float v_dot_h)
{
	float f = 1.0 - v_dot_h;
	float f2 = f * f;
	return f0 + (1.0 - f0) * f2 * f2 * f;
}


vec3 CalculateLighting(vec3 base_color, float roughness, float F0,
						vec3 n, vec3 v, vec3 l, vec3 h)
{

	float n_dot_l = clamp(dot(n, l), 0.0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0.0, 1.0);
	float n_dot_v = clamp(dot(n, v), 0.0, 1.0);
	float v_dot_h = clamp(dot(v, h), 0.0, 1.0);


	float D = BRDF_SpecularD_GGX(roughness, n_dot_h);
	float G = BRDF_SpecularG_Schlick(roughness, n_dot_l, n_dot_v);
	float F = BRDF_SpecularF_Schlick(F0, v_dot_h);

	float specular = F * G * D;
	specular /= 4.0 * n_dot_v; // * n_dot_l

	vec3 color = specular * base_color; // * n_dot_l // specular

	color += (1.0 - F) * base_color * n_dot_l; // diffuse

	return color;
}

vec3 PointLightLighting(vec3 position, vec3 material_diffuse, vec4 material_specular, vec3 cam_dir, vec3 normal,
						vec3 light_pos, float point_light_dist, vec3 light_color, // TODO: better name for point_light_dist
						bool shadow_enabled, samplerCube shadow_map)
{
	float roughness = 0.5;
	float F0 = 0.3;


	vec3 point_to_light = light_pos - position;
	float light_dist2 = dot(point_to_light, point_to_light);

	if(light_dist2 > point_light_dist * point_light_dist)
		return vec3(0.0);

	float light_dist = sqrt(light_dist2); // real distance


	vec3 l = point_to_light / light_dist;
	vec3 v = normalize(cam_dir); // TODO: should be normalized before
	vec3 n = normalize(normal); // TODO: should be normalized before
	vec3 h = normalize(v + l);




	float shadow = 1.0;

	if(shadow_enabled)
	{
		vec2 moments = texture(shadow_map, -l).rg;

		float light_depth = light_dist - 0.01;

		if(light_depth <= moments.x)
			shadow = 1.0;
		else
		{
			float p = smoothstep(light_depth-0.00005, light_depth, moments.x);
			float variance = max(moments.y - moments.x*moments.x, -0.001);
			float d = light_depth - moments.x;
			float p_max = linstep(0.3, 1.0, variance / (variance + d*d));

			shadow = p_max; //clamp(max(p, p_max), 0.0, 1.0);
		}
	}
	else
		shadow = 1.0;


    float light_dist_attenuation = (1.0 - light_dist / point_light_dist);
    float light_intensity = max(dot(n, l), 0.0) * light_dist_attenuation;

    vec3 radiance = light_intensity * shadow * light_color;

	return radiance * CalculateLighting(material_diffuse, roughness, F0, n, v, l, h);
}
