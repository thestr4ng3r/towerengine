#version 330

#define MAX_DIRECTIONAL_SHADOW_SPLITS 8
#define DIRECTIONAL_LIGHT_SHADOW_LAYER_BLEND_DIST 0.5

uniform vec3 cam_pos_uni;

uniform vec3 directional_light_dir_uni;
uniform vec3 directional_light_color_uni;
uniform bool directional_light_shadow_enabled_uni;
uniform vec2 directional_light_shadow_clip_uni;
uniform mat4 directional_light_shadow_tex_matrix_uni[MAX_DIRECTIONAL_SHADOW_SPLITS];
uniform int directional_light_shadow_splits_count_uni;
uniform float directional_light_shadow_splits_z_uni[MAX_DIRECTIONAL_SHADOW_SPLITS+1];
uniform sampler2DArray directional_light_shadow_map_uni;

uniform sampler2D position_tex_uni;
uniform sampler2D diffuse_tex_uni;
uniform sampler2D normal_tex_uni;
uniform sampler2D specular_tex_uni;

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
	vec4 specular = texture2D(specular_tex_uni, uv_coord_var).rgba;
	
	
	vec3 cam_dir = normalize(cam_pos_uni - position.xyz);
	
	float shadow = 1.0;	

	if(directional_light_shadow_enabled_uni)
	{ 
		vec2 coord = vec2(0.0, 0.0);
		int layer = -1;
		
		for(int s=0; s<directional_light_shadow_splits_count_uni; s++)
		{
			coord = ((directional_light_shadow_tex_matrix_uni[s] * vec4(position.xyz, 1.0)).xy * 0.5) + vec2(0.5, 0.5);
			
			if(!(coord.x < 0.0 || coord.x > 1.0 || coord.y < 0.0 || coord.y > 1.0))
			{	
				layer = s;				
				break;
			}
		}
		
		if(layer != -1)
		{
			vec2 moments = texture(directional_light_shadow_map_uni, vec3(coord, float(layer))).rg;
										
			float light_dot = dot(position.xyz - cam_pos_uni, -directional_light_dir_uni) / (directional_light_shadow_clip_uni.y - directional_light_shadow_clip_uni.x);

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
		
		
	float light_intensity = max(dot(normal, directional_light_dir_uni), 0.0);
	vec3 color = shadow * light_intensity * directional_light_color_uni * diffuse.rgb; // diffuse light

	//specular
	vec3 specular_color = specular.rgb * directional_light_color_uni;
	float specular_intensity = max(dot(normalize(reflect(-directional_light_dir_uni, normal)), cam_dir), 0.0);
	color += max(vec3(0.0, 0.0, 0.0), specular_color * pow(specular_intensity, specular.a)) * shadow;
	
	
	gl_FragColor = vec4(color, 1.0);
}
