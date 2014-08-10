#version 130

in float moment1_var;

out vec4 gl_FragColor;

void main(void)
{	
	// Adjusting moments (this is sort of bias per pixel) using partial derivative
	float dx = dFdx(moment1_var);
	float dy = dFdy(moment1_var);
	float moment2 = (moment1_var * moment1_var) + 0.25*(dx*dx+dy*dy);
	
	gl_FragColor = vec4(moment1_var, moment2, 0.0, 1.0);
}
