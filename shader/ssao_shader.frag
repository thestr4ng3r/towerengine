#version 330

#extension GL_ARB_shading_language_include : require

#include "position_restore.glsl"

#define MAX_KERNEL_SIZE 32

uniform vec3 kernel_uni[MAX_KERNEL_SIZE];
uniform int kernel_size_uni;

uniform sampler2D noise_tex_uni;
uniform vec2 noise_tex_scale_uni;

uniform sampler2D depth_tex_uni;
uniform sampler2D normal_tex_uni;

uniform mat4 projection_matrix_uni;
uniform mat4 modelview_matrix_uni;

uniform float radius_uni;

uniform vec3 cam_pos_uni;
uniform vec3 cam_dir_uni;

in vec2 uv_coord_var;

out vec4 color_out;


void main(void)
{
	mat4 transpose_modelview = transpose(modelview_matrix_uni);
	vec3 origin = CalculateWorldPosition(texture(depth_tex_uni, uv_coord_var).x, uv_coord_var);
		
	vec3 normal = texture(normal_tex_uni, uv_coord_var).rgb * 2.0 - vec3(1.0, 1.0, 1.0);
	normal = normalize(normal);
	
	vec3 rvec = texture(noise_tex_uni, uv_coord_var * noise_tex_scale_uni).rgb * 2.0 - 1.0;
	vec3 tang = normalize(rvec - normal * dot(rvec, normal));
	vec3 bitang = cross(normal, tang);
	mat3 tang_mat = mat3(tang, bitang, normal);
	
	float occlusion = 0.0;
	
	for(int i=0; i<kernel_size_uni; i++)
	{
		vec3 s = tang_mat * kernel_uni[i];
		s = s * radius_uni + origin;
		
		float sample_z = (vec4(s, 1.0) * transpose_modelview).z; //dot(sample - cam_pos_uni, cam_dir_uni);
		
		vec4 offset = vec4(s, 1.0) * transpose_modelview;
		offset = projection_matrix_uni * offset;
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;
		
		if(offset.x > 1.0 || offset.y > 1.0 || offset.x < 0.0 || offset.y < 0.0)
			continue;

		vec3 sample_pos = CalculateWorldPosition(texture(depth_tex_uni, offset.xy).x, offset.xy);
		float sample_real_z = (vec4(sample_pos, 1.0) * transpose_modelview).z;//dot(sample_pos - cam_pos_uni, cam_dir_uni);
		
		float range_check = abs(sample_real_z - sample_z) < radius_uni ? 1.0 : 0.0;
		occlusion += sample_real_z < sample_z ? 0.0 : 1.0 * range_check;
	}
		
	occlusion = 1.0 - (occlusion / float(kernel_size_uni));
			
	color_out = vec4(occlusion, 0.0, 0.0, 1.0);
}