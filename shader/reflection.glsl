

#extension GL_ARB_texture_query_lod : enable
#extension GL_ARB_texture_query_levels : enable


uniform samplerCube reflection_tex_uni;


vec3 GetReflectionColor(float reflectance, float roughness, vec3 normal, vec3 cam_dir)
{
	vec3 cam_reflected = reflect(-cam_dir, normal);

#if GL_ARB_texture_query_levels
	int mipmap_levels = textureQueryLevels(reflection_tex_uni);
#else
	int mipmap_levels = log2(textureSize(reflection_tex_uni).x);
#endif

#if GL_ARB_texture_query_lod
	float mipmap_level = textureQueryLOD(reflection_tex_uni, cam_reflected).x;
#else
	float mipmap_level = 0.0;
#endif

	float blur_mipmap_level = float(mipmap_levels) * pow(roughness, 0.6);
	mipmap_level = max(mipmap_level, blur_mipmap_level);

	vec3 reflection_color = textureLod(reflection_tex_uni, cam_reflected, mipmap_level).rgb;
	return reflection_color * reflectance;
}