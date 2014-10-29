#version 330

uniform vec3 diffuse_color_uni;

out vec4 gl_FragColor;

void main(void)
{
	if(!gl_FrontFacing)
		discard;

	gl_FragColor = vec4(diffuse_color_uni, 1.0);
}
