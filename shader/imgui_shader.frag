#version 330

uniform sampler2D tex_uni;

in vec2 uv_var;
in vec4 color_var;

out vec4 color_out;

void main(void)
{
	color_out = color_var * texture(tex_uni, uv_var);
}