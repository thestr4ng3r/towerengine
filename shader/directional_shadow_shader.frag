#version 130

in float moment1_var;

out vec4 gl_FragColor;

void main(void)
{	
	gl_FragColor = vec4(moment1_var, moment1_var * moment1_var, 0.0, 1.0);
}
