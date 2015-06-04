#version 330

in vec4 color_var;

out vec4 color_out;

void main(void)
{
	if(!gl_FrontFacing)
		discard;

	color_out = color_var;
}
