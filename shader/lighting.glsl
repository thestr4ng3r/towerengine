



float linstep(float min, float max, float v)
{
	return clamp((v - min) / (max - min), 0.0, 1.0);
}


vec3 PointLightLighting(in vec3 position, in vec3 material_diffuse, in vec4 material_specular, in vec3 cam_dir, in vec3 normal,
						in vec3 light_pos, in float point_light_dist, vec3 light_color, // TODO: better name for point_light_dist
						in bool shadow_enabled, in samplerCube shadow_map)
{
    float shadow = 1.0;

    vec3 light_dir = light_pos - position;

    float light_dist_sq = light_dir.x * light_dir.x + light_dir.y * light_dir.y + light_dir.z * light_dir.z; // squared distance

    if(light_dist_sq > point_light_dist * point_light_dist)
        return vec3(0.0);



    float light_dist = sqrt(light_dist_sq); // real distance
    light_dir /= light_dist; // normalized dir

    if(shadow_enabled)
    {
        vec2 moments = texture(shadow_map, -light_dir).rg;
        //vec2 moments = vec2(0.0);

        float light_depth = light_dist - 0.01;

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


    float light_dist_attenuation = (1.0 - light_dist / point_light_dist);
    float light_intensity = max(dot(normal, light_dir), 0.0) * light_dist_attenuation;
    vec3 color = shadow * light_intensity * light_color * material_diffuse; // diffuse light

    //specular
    vec3 specular_color = material_specular.rgb * light_color;
    float specular_intensity = max(dot(normalize(reflect(-light_dir, normal)), cam_dir), 0.0); // TODO: reflect cam_dir instead of light_dir to avoid recalculating
    color += max(vec3(0.0, 0.0, 0.0), specular_color * pow(specular_intensity, material_specular.a)) * shadow * light_dist_attenuation;

    return color;
}