#version 330

uniform vec4 color_uni;

out vec4 color_out;

void main(void)
{
	color_out = color_uni;
}