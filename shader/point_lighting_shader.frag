#version 330

#extension GL_ARB_shading_language_include : require

#include "position_restore.glsl"
#include "lighting.glsl"

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



	vec3 color = vec3(0.0, 0.0, 0.0);

	int i;
	#pragma for(i from 0 ex param lights_count)
		color += PointLightLighting(position.xyz,
									diffuse.rgb,
									specular.rgba,
									cam_dir,
									normal,
									point_light_pos_uni[i],
									point_light_distance_uni[i],
									point_light_color_uni[i],
									point_light_shadow_enabled_uni[i],
									point_light_shadow_map_uni[i]);
	#pragma endfor
	
	color_out = vec4(color, 1.0);
}
