#version 130

out vec4 gl_FragColor;

uniform samplerCube tex_uni;

uniform vec3 blur_dir_uni;

in vec3 pos_var;

void main()
{
	vec3 coord = normalize(pos_var);
	vec2 color = vec2(0.0, 0.0);
	
	color += texture(tex_uni, coord - blur_dir_uni * 4.0).rg * 0.05;
	color += texture(tex_uni, coord - blur_dir_uni * 3.0).rg * 0.09;
	color += texture(tex_uni, coord - blur_dir_uni * 2.0).rg * 0.12;
	color += texture(tex_uni, coord - blur_dir_uni).rg * 0.15;
	color += texture(tex_uni, coord).rg * 0.16;
	color += texture(tex_uni, coord + blur_dir_uni).rg * 0.15;
	color += texture(tex_uni, coord + blur_dir_uni * 2.0).rg * 0.12;
	color += texture(tex_uni, coord + blur_dir_uni * 3.0).rg * 0.09;
	color += texture(tex_uni, coord + blur_dir_uni * 4.0).rg * 0.05;
	
	gl_FragColor = vec4(color, 0.0, 1.0);
}