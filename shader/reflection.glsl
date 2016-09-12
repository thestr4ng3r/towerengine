

#extension GL_ARB_texture_query_lod : enable
#extension GL_ARB_texture_query_levels : enable


uniform samplerCube reflection_tex1_uni;
uniform samplerCube reflection_tex2_uni;
uniform float reflection_tex_blend_uni;


vec3 QueryReflectionColor(samplerCube reflection_tex, float roughness, vec3 dir)
{
#if GL_ARB_texture_query_levels
	int mipmap_levels = textureQueryLevels(reflection_tex);
#else
	int mipmap_levels = log2(textureSize(reflection_tex).x);
#endif

#if GL_ARB_texture_query_lod
	float mipmap_level = textureQueryLOD(reflection_tex, dir).x;
#else
	float mipmap_level = 0.0;
#endif

	float blur_mipmap_level = float(mipmap_levels) * pow(roughness, 0.3);
	mipmap_level = max(mipmap_level, blur_mipmap_level);

	vec3 reflection_color = textureLod(reflection_tex, dir, mipmap_level).rgb;
	return reflection_color;
}

vec3 GetReflectionColor(float reflectance, float roughness, vec3 normal, vec3 cam_dir)
{
	vec3 cam_reflected = reflect(-cam_dir, normal);
	vec3 color = QueryReflectionColor(reflection_tex1_uni, roughness, cam_reflected);

	if(reflection_tex_blend_uni > 0.0)
	{
		vec3 color2 = QueryReflectionColor(reflection_tex2_uni, roughness, cam_reflected);
		color = reflection_tex_blend_uni * color + (1.0 - reflection_tex_blend_uni) * color2;
	}

	return color * reflectance;
}