#version 130

uniform vec3 light_pos;
uniform float light_dist;

in vec3 pos_var;

out vec4 gl_FragColor;

void main(void)
{
	vec3 v = pos_var - light_pos;
	float d = v.x*v.x + v.y*v.y + v.z*v.z;
	
	if(d > light_dist * light_dist)
		discard;
		
	d = sqrt(d) / light_dist;

	float moment1 = d;
	float moment2 = d * d;

	// Adjusting moments (this is sort of bias per pixel) using partial derivative
	float dx = dFdx(d);
	float dy = dFdy(d);
	moment2 += 0.25*(dx*dx+dy*dy) ;
	
	gl_FragColor = vec4(moment1,moment2, 0.0, 1.0);
}














