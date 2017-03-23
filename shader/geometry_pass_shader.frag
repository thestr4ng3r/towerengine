#version 330


layout(std140) uniform MaterialBlock
{
	uniform vec3 base_color;
	uniform float metallic;

	uniform vec3 emission;
	uniform float roughness;

	uniform float reflectance;
	uniform float bump_depth;
	uniform bool base_color_tex_enabled;
	uniform bool metallic_roughness_tex_enabled;

	uniform bool normal_tex_enabled;
	uniform bool bump_tex_enabled;
	uniform bool emission_tex_enabled;
} material_uni;



uniform sampler2D base_color_tex_uni;
uniform sampler2D metallic_roughness_tex_uni;
uniform sampler2D normal_tex_uni;
uniform sampler2D bump_tex_uni;
uniform sampler2D emission_tex_uni;




in vec3 pos_var;
in vec3 normal_var;
in vec3 tang_var;
in vec3 bitang_var;
in vec2 uv_var;

in vec3 cam_dir_var;

in vec3 reflection_center_var;
in float reflection_radius_var;

layout (location = 0) out vec4 base_color_out;
layout (location = 1) out vec4 normal_out;
layout (location = 2) out vec4 face_normal_out;
layout (location = 3) out vec4 metal_rough_reflect_out;
layout (location = 4) out vec4 emission_out;

vec2 ParallaxUV(void);
vec2 ParallaxOcclusionUV(mat3 tang_mat);

void main(void)
{
	//if(!gl_FrontFacing) // backface culling
	//	discard;
	

	mat3 tang_mat = mat3(normalize(tang_var), normalize(bitang_var), normalize(normal_var));
	
	vec2 uv;
	
	if(material_uni.bump_tex_enabled)
		uv = ParallaxOcclusionUV(tang_mat);
	else
		uv = uv_var;
				
	
	// base_color
	
	vec4 base_color;
	if(material_uni.base_color_tex_enabled)
	{
		base_color = texture(base_color_tex_uni, uv).rgba;
		if(base_color.a < 0.5)
			discard;
	}
	else
		base_color = vec4(material_uni.base_color.rgb, 1.0);


	// metallic, roughness, reflectance

	float metallic;
	float roughness;
	float reflectance;

	if(material_uni.metallic_roughness_tex_enabled)
	{
		vec3 metal_rough_reflect = texture(metallic_roughness_tex_uni, uv).rgb;
		metallic = metal_rough_reflect.r;
		roughness = metal_rough_reflect.g;
		reflectance = metal_rough_reflect.b;
	}
	else
	{
		metallic = material_uni.metallic;
		roughness = material_uni.roughness;
		reflectance = material_uni.reflectance;
	}
	
	
	//normal
	
	vec3 normal;
	if(material_uni.normal_tex_enabled)
	{
		vec3 normal_tex_color = texture(normal_tex_uni, uv).rgb;
		normal = tang_mat * ((normal_tex_color - vec3(0.5, 0.5, 0.5)) * 2.0);
	}
	else
		normal = normal_var;
		
	normal_out = vec4(normal * 0.5 + vec3(0.5, 0.5, 0.5), 1.0);
	
	face_normal_out = vec4(normal_var * 0.5 + vec3(0.5, 0.5, 0.5), 1.0);
	

	
		
	// emission
	
	vec3 emission;
	
	if(material_uni.emission_tex_enabled)
		emission = texture(emission_tex_uni, uv).rgb;
	else
		emission = material_uni.emission;


		
	
	// out

	base_color_out = base_color;
	metal_rough_reflect_out = vec4(metallic, roughness, reflectance, 0.0);
	emission_out = vec4(emission, 1.0);
}


vec2 ParallaxUV(void)
{
	float height = texture(bump_tex_uni, uv_var).r * material_uni.bump_depth;
	vec3 cam_offset = height * normalize(cam_dir_var);
	vec2 uv_offset;
	uv_offset.x = dot(cam_offset, tang_var);
	uv_offset.y = dot(cam_offset, bitang_var);
	return uv_var + uv_offset;
}

#define PARALLAX_OCCLUSION_MIN_SAMPLES 4
#define PARALLAX_OCCLUSION_MAX_SAMPLES 15

vec2 ParallaxOcclusionUV(mat3 tang_mat)
{
	vec3 cam_dir_tang = cam_dir_var * tang_mat;
	
	float parallax_limit = -length(cam_dir_tang.xy) / cam_dir_tang.z;
	parallax_limit *= material_uni.bump_depth;
	
	vec2 offset_dir = normalize(cam_dir_tang.xy);
		
	vec2 max_offset = offset_dir * parallax_limit;
	
	int samples_count = int(mix(PARALLAX_OCCLUSION_MAX_SAMPLES, PARALLAX_OCCLUSION_MIN_SAMPLES, dot(-normalize(cam_dir_var), normal_var)));
	float step_size = 1.0 / float(samples_count);
	
	vec2 dx = dFdx(uv_var);
	vec2 dy = dFdy(uv_var);
	
	float current_ray_height = 1.0;
	vec2 current_offset = vec2(0.0, 0.0);
	vec2 last_offset = vec2(0.0, 0.0);
	
	float last_sampled_height = 1.0;
	float current_sampled_height = 1.0;
	
	int current_sample = 0;
	
	while(current_sample < samples_count)
	{
		current_sampled_height = textureGrad(bump_tex_uni, uv_var + current_offset, dx, dy).r;
		
		if(current_sampled_height > current_ray_height)
		{
			float delta1 = current_sampled_height - current_ray_height;
			float delta2 = (current_ray_height + step_size) - last_sampled_height;
			float ratio = delta1 / (delta1 + delta2);
			
			current_offset = ratio * last_offset + (1.0 - ratio) * current_offset;
			
			break;
		}		
		else
		{
			current_sample++;
			
			current_ray_height -= step_size;
			last_offset = current_offset;
			current_offset += step_size * max_offset;
			
			last_sampled_height = current_sampled_height;
		}
	}
	
	return uv_var + current_offset;
}
