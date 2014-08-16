#version 130

out vec4 gl_FragColor;

uniform sampler2DArray tex_uni;

uniform vec2 blur_dir_uni;
uniform float tex_layer_uni;

in vec2 uv_coord_var;

void main()
{
	vec2 color = vec2(0.0, 0.0);
	
	color += texture(tex_uni, vec3(uv_coord_var - blur_dir_uni * 5.0, tex_layer_uni)).rg * 0.01;
	color += texture(tex_uni, vec3(uv_coord_var - blur_dir_uni * 4.0, tex_layer_uni)).rg * 0.05;
	color += texture(tex_uni, vec3(uv_coord_var - blur_dir_uni * 3.0, tex_layer_uni)).rg * 0.09;
	color += texture(tex_uni, vec3(uv_coord_var - blur_dir_uni * 2.0, tex_layer_uni)).rg * 0.12;
	color += texture(tex_uni, vec3(uv_coord_var - blur_dir_uni, tex_layer_uni)).rg * 0.15;
	color += texture(tex_uni, vec3(uv_coord_var, tex_layer_uni)).rg * 0.16;
	color += texture(tex_uni, vec3(uv_coord_var + blur_dir_uni, tex_layer_uni)).rg * 0.15;
	color += texture(tex_uni, vec3(uv_coord_var + blur_dir_uni * 2.0, tex_layer_uni)).rg * 0.12;
	color += texture(tex_uni, vec3(uv_coord_var + blur_dir_uni * 3.0, tex_layer_uni)).rg * 0.09;
	color += texture(tex_uni, vec3(uv_coord_var + blur_dir_uni * 4.0, tex_layer_uni)).rg * 0.05;
	color += texture(tex_uni, vec3(uv_coord_var + blur_dir_uni * 5.0, tex_layer_uni)).rg * 0.01;
	
	gl_FragColor = vec4(color, 0.0, 1.0);
}