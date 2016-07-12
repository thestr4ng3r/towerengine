#version 330

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shading_language_include : require

#include "position_restore.glsl"
#include "lighting.glsl"

uniform vec3 cam_pos_uni;

uniform sampler2D depth_tex_uni;
uniform sampler2D diffuse_tex_uni;
uniform sampler2D normal_tex_uni;
uniform sampler2D specular_tex_uni;
uniform sampler2D self_illumination_tex_uni;

uniform bool ssao_enabled_uni;
uniform sampler2D ssao_tex_uni;

in vec2 uv_coord_var;

out vec4 color_out;

// ambient

uniform vec3 light_ambient_color_uni;

// point lights

#pragma define_param(MAX_POINT_LIGHTS_COUNT max_point_lights_count)

struct PointLight
{
	float dist;
	int shadow_enabled;
	vec3 pos;
	vec3 color;
	samplerCube shadow_map;
};


layout(std140) uniform PointLightBlock
{	
	int count;
	PointLight light[MAX_POINT_LIGHTS_COUNT];
} point_light_uni;






void main(void)
{	
	float depth = texture(depth_tex_uni, uv_coord_var).x;
	
	if(depth == 1.0)
		discard;

	vec3 diffuse = texture(diffuse_tex_uni, uv_coord_var).rgb;
	vec3 position = CalculateWorldPosition(depth, uv_coord_var);
	vec3 normal = normalize(texture(normal_tex_uni, uv_coord_var).rgb * 2.0 - vec3(1.0, 1.0, 1.0));
	vec4 specular = texture(specular_tex_uni, uv_coord_var).rgba;
		
	vec3 cam_dir = normalize(cam_pos_uni - position.xyz);
	
	
	
	// ambient lighting, ssao and self illumination
	
	vec3 color = diffuse.rgb * light_ambient_color_uni;
	
	if(ssao_enabled_uni)
	{
		float occlusion = texture(ssao_tex_uni, uv_coord_var).r;
		color *= occlusion;
	}
	
	vec3 self_illumination = texture(self_illumination_tex_uni, uv_coord_var).rgb;
	color += self_illumination;
	
	
	// point lighting
	
	for(int i=0; i<point_light_uni.count; i++)
	{
		color += PointLightLighting(position.xyz,
									diffuse.rgb,
									specular.rgba,
									cam_dir,
									normal,
									point_light_uni.light[i].pos,
									point_light_uni.light[i].dist,
									point_light_uni.light[i].color,
									point_light_uni.light[i].shadow_enabled != 0,
									point_light_uni.light[i].shadow_map);
	}
	
	color_out = vec4(color, 1.0);
}