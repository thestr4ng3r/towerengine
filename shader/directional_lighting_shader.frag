#version 330

#extension GL_ARB_shading_language_include : require

#include "position_restore.glsl"
#include "lighting.glsl"



uniform vec3 cam_pos_uni;

uniform vec3 directional_light_dir_uni;
uniform vec3 directional_light_color_uni;
uniform bool directional_light_shadow_enabled_uni;
uniform vec2 directional_light_shadow_clip_uni;
uniform mat4 directional_light_shadow_tex_matrix_uni[MAX_DIRECTIONAL_SHADOW_SPLITS];
uniform int directional_light_shadow_splits_count_uni;
uniform float directional_light_shadow_splits_z_uni[MAX_DIRECTIONAL_SHADOW_SPLITS+1];
uniform sampler2DArray directional_light_shadow_map_uni;

uniform sampler2D depth_tex_uni;
uniform sampler2D base_color_tex_uni;
uniform sampler2D normal_tex_uni;
uniform sampler2D metallic_roughness_tex_uni;

in vec2 uv_coord_var;

out vec4 color_out;



void main(void)
{
	float depth = texture(depth_tex_uni, uv_coord_var).x;
	
	if(depth == 1.0)
		discard;
	
	ivec2 texel_uv = ivec2(uv_coord_var * textureSize(base_color_tex_uni, 0).xy);
	
	vec3 base_color = texelFetch(base_color_tex_uni, texel_uv, 0).rgb;
	vec3 position = CalculateWorldPosition(depth, uv_coord_var);
	vec3 normal = normalize(texelFetch(normal_tex_uni, texel_uv, 0).rgb * 2.0 - vec3(1.0, 1.0, 1.0));
	vec2 metallic_roughness = texelFetch(metallic_roughness_tex_uni, texel_uv, 0).rg;
	float metallic = metallic_roughness.r;
	float roughness = metallic_roughness.g;


	vec3 color = DirectionalLightLighting(position, base_color, metallic, roughness,
											cam_pos_uni, normal,
											directional_light_dir_uni, directional_light_color_uni,
											directional_light_shadow_enabled_uni,
											directional_light_shadow_splits_count_uni,
											directional_light_shadow_clip_uni,
											directional_light_shadow_tex_matrix_uni,
											directional_light_shadow_splits_z_uni,
											directional_light_shadow_map_uni);
	
	color_out = vec4(color, 1.0);
}
