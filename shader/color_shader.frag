#version 330

in vec4 color_var;

out vec4 gl_FragColor;

void main(void)
{
	if(!gl_FrontFacing)
		discard;

	gl_FragColor = color_var;
}
