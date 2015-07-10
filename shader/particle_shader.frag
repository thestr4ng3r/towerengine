#version 330

uniform vec4 color_uni;
uniform sampler2D texture_uni;

in vec2 uv_var;

out vec4 color_out;

void main(void)
{
	color_out = color_uni * texture(texture_uni, uv_var);
}