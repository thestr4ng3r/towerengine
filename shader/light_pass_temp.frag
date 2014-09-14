

void main(void)
{
	vec3 cam_dir = cam_pos_var - pos_var;
	float cam_dist = length(cam_dir);
	cam_dir /= cam_dist;
	
	vec3 color = light_ambient_color_uni * diffuse_color.rgb * ambient_uni; // ambient
	
	float light_intensity;
	float shadow = 1.0;
	vec3 specular_color;
	float specular_intensity;
	int i;
	float bias;
	float shadow_depth, light_dist_sq;
	
	for(i=0; i<point_light_count_uni; i++)
	{
		vec3 light_dir = point_light_pos_uni[i] - pos_var; // pos to light
		light_dist_sq = light_dir.x * light_dir.x + light_dir.y * light_dir.y + light_dir.z * light_dir.z; // squared distance
		if(light_dist_sq > point_light_distance_uni[i] * point_light_distance_uni[i])
			continue;
		float light_dist = sqrt(light_dist_sq); // real distance
		light_dir /= light_dist; // normalized dir
		
		if(point_light_shadow_enabled_uni[i])
		{ 
			vec2 moments = PointLightShadowLookup(i, -light_dir).rg;
			
			float light_depth = length(point_light_pos_uni[i] - pos_var) / point_light_distance_uni[i];//light_dist / point_light_distance_uni[i];
									
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
		color += shadow * light_intensity * point_light_color_uni[i] * diffuse_color.rgb; // diffuse light
	
		//specular
		specular_color = specular_tex_color * specular_color_uni * point_light_color_uni[i];
		specular_intensity = max(dot(normalize(reflect(-light_dir, normal)), cam_dir), 0.0);// * light_intensity;
		color += max(vec3(0.0, 0.0, 0.0), specular_color * pow(specular_intensity, specular_size_uni)) * shadow;
	}
	
	for(i=0; i<directional_light_count_uni; i++)
	{		
		if(directional_light_shadow_enabled_uni[i])
		{ 
			vec2 coord = vec2(0.0, 0.0);
			int layer = -1;
			
			for(int s=0; s<directional_light_shadow_splits_count_uni[i]; s++)
			{
				coord = ((directional_light_shadow_tex_matrix_uni[i*MAX_DIRECTIONAL_SHADOW_SPLITS + s] * vec4(pos_var, 1.0)).xy * 0.5) + vec2(0.5, 0.5);
				
				if(!(coord.x < 0.0 || coord.x > 1.0 || coord.y < 0.0 || coord.y > 1.0))
				{	
					layer = s;				
					break;
				}
			}
			
			if(layer != -1)
			{
				vec2 moments = DirectionalLightShadowLookup(i, vec3(coord, float(layer))).rg;
											
				float light_dot = (dot(pos_var - cam_pos_var, -directional_light_dir_uni[i]) + directional_light_shadow_clip_uni[i].x) /
									(directional_light_shadow_clip_uni[i].y - directional_light_shadow_clip_uni[i].x);

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
		color += shadow * light_intensity * directional_light_color_uni[i] * diffuse_color.rgb; // diffuse light
	
		//specular
		specular_color = specular_tex_color * specular_color_uni * directional_light_color_uni[i];
		specular_intensity = max(dot(normalize(reflect(-directional_light_dir_uni[i], normal)), cam_dir), 0.0);// * light_intensity;
		color += max(vec3(0.0, 0.0, 0.0), specular_color * pow(specular_intensity, specular_size_uni)) * shadow;
	}
	gl_FragColor = vec4(color, alpha) * diffuse_color2_uni;*/
}