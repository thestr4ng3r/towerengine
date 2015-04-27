#version 130

out vec4 tex_out[6];

uniform samplerCube tex_uni;

in vec3 coords_var[6];
in vec3 blur_dir_var[6];

void main()
{
	vec2 color;

	for(int s=0; s<6; s++)
	{
		color = vec2(0.0, 0.0);
		
		color += texture(tex_uni, coords_var[s] - blur_dir_var[s] * 2.0).rg * 0.2;
		color += texture(tex_uni, coords_var[s] - blur_dir_var[s]).rg * 0.2;
		color += texture(tex_uni, coords_var[s]).rg * 0.2;
		color += texture(tex_uni, coords_var[s] + blur_dir_var[s]).rg * 0.2;
		color += texture(tex_uni, coords_var[s] + blur_dir_var[s] * 2.0).rg * 0.2;
		
		tex_out[s] = vec4(color, 0.0, 1.0);
	}
}