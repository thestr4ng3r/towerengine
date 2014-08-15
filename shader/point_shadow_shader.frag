#version 130

in vec3 pos_var;

uniform vec3 light_pos_uni;
uniform float light_dist_uni;

out vec4 gl_FragColor;

void main(void)
{
	vec2 moments;
	
	moments.x = length(pos_var - light_pos_uni) / light_dist_uni;
	if(moments.x > 1.0)
		discard;
	moments.y = moments.x * moments.x;

	// Adjusting moments (this is sort of bias per pixel) using partial derivative
	float dx = dFdx(moments.x);
	float dy = dFdy(moments.x);
	moments.y += 0.25*(dx*dx+dy*dy) ;
	
	gl_FragColor = vec4(moments, 0.0, 1.0);
}














