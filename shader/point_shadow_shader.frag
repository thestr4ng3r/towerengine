#version 130

in vec3 pos_var;

uniform vec3 light_pos_uni;
uniform float light_dist_uni;

out vec4 gl_FragColor;

void main(void)
{
	vec3 dir = pos_var - light_pos_uni;
	float dist = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
	
	if(dist > light_dist_uni * light_dist_uni)
		discard;
	
	dist = sqrt(dist);
	
	gl_FragColor = vec4(dist, dist*dist, 0.0, 1.0);
}














