// #version 330

#include "position_restore.glsl"
#include "reflection.glsl"

uniform vec3 light_ambient_color_uni;

uniform vec3 cam_pos_uni;

uniform sampler2D depth_tex_uni;
uniform sampler2D base_color_tex_uni;
uniform sampler2D normal_tex_uni;
uniform sampler2D metal_rough_reflect_tex_uni;
uniform sampler2D emission_tex_uni;

#if AMBIENT_SSAO
uniform sampler2D ssao_tex_uni;
#endif


in vec2 uv_coord_var;

out vec4 color_out;


void main(void)
{
	float depth = texture(depth_tex_uni, uv_coord_var).x;
	if(depth == 1.0)
		discard;

	vec3 base_color = texture(base_color_tex_uni, uv_coord_var).rgb;
	vec3 position = CalculateWorldPosition(depth, uv_coord_var);
	vec3 normal = normalize(texture(normal_tex_uni, uv_coord_var).rgb * 2.0 - vec3(1.0));
	vec3 metal_rough_reflect = texture(metal_rough_reflect_tex_uni, uv_coord_var).rgb;
	float metallic = metal_rough_reflect.r;
	float roughness = metal_rough_reflect.g;
	float reflectance = metal_rough_reflect.b;

	vec3 cam_dir = normalize(cam_pos_uni - position.xyz);


	vec3 color = vec3(0.0);

	// ambient

	color += base_color.rgb * light_ambient_color_uni;


	// reflection

	color += GetReflectionColor(reflectance, roughness, normal, cam_dir) * metallic;


	//ssao

#if AMBIENT_SSAO
	float occlusion = texture(ssao_tex_uni, uv_coord_var).r;
	color *= occlusion;
#endif


	// emission

	vec3 emission = texture(emission_tex_uni, uv_coord_var).rgb;
	color += emission;


	color_out = vec4(color, 1.0);
}