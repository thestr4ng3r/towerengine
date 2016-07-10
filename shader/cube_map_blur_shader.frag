#version 330

uniform samplerCube tex_uni;
uniform int mipmap_level_uni;

in vec3 coords_var[6];
in vec3 blur_dir_var[6];

out vec4 tex_out[6];

void main()
{
	vec3 color;

	for(int s=0; s<6; s++)
	{
		color = vec3(0.0);
		
		/*color += textureLod(tex_uni, coords_var[s] - blur_dir_var[s] * 2.0, mipmap_level_uni).rgb * 0.06136;
		color += textureLod(tex_uni, coords_var[s] - blur_dir_var[s], 		mipmap_level_uni).rgb * 0.24477;
		color += textureLod(tex_uni, coords_var[s], 						mipmap_level_uni).rgb * 0.38774;
		color += textureLod(tex_uni, coords_var[s] + blur_dir_var[s], 		mipmap_level_uni).rgb * 0.24477;
		color += textureLod(tex_uni, coords_var[s] + blur_dir_var[s] * 2.0, mipmap_level_uni).rgb * 0.06136;*/

		color += textureLod(tex_uni, coords_var[s] - blur_dir_var[s] * 2.0, mipmap_level_uni).rgb * 0.00598;
		color += textureLod(tex_uni, coords_var[s] - blur_dir_var[s] * 3.0, mipmap_level_uni).rgb * 0.060626;
		color += textureLod(tex_uni, coords_var[s] - blur_dir_var[s], 		mipmap_level_uni).rgb * 0.241843;
		color += textureLod(tex_uni, coords_var[s], 						mipmap_level_uni).rgb * 0.383103;
		color += textureLod(tex_uni, coords_var[s] + blur_dir_var[s], 		mipmap_level_uni).rgb * 0.241843;
		color += textureLod(tex_uni, coords_var[s] + blur_dir_var[s] * 2.0, mipmap_level_uni).rgb * 0.060626;
		color += textureLod(tex_uni, coords_var[s] + blur_dir_var[s] * 3.0, mipmap_level_uni).rgb * 0.00598;


		tex_out[s] = vec4(color, 1.0);
	}
}