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



out vec4 color_out;

void main(void)
{
	mat3 tang_mat = mat3(normalize(tang_var), normalize(bitang_var), normalize(normal_var));

	vec2 uv;

	/*if(material_uni.bump_tex_enabled)
		uv = ParallaxOcclusionUV(tang_mat);
	else*/
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

	//normal_out = vec4(normal * 0.5 + vec3(0.5, 0.5, 0.5), 1.0);

	//face_normal_out = vec4(normal_var * 0.5 + vec3(0.5, 0.5, 0.5), 1.0);




	// emission

	vec3 emission;

	if(material_uni.emission_tex_enabled)
		emission = texture(emission_tex_uni, uv).rgb;
	else
		emission = material_uni.emission;


	color_out = base_color;
}