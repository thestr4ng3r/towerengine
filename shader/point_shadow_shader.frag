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
		
	gl_FragColor = vec4(sqrt(d) / light_dist, 0.0, 0.0, 1.0);
}














