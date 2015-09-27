#version 330

uniform samplerCube cube_map_uni;

in vec3 pos_var;

out vec4 color_out;

void main(void)
{
	vec3 color = texture(cube_map_uni, normalize(pos_var)).xyz;
	color_out = vec4(color, 0.0);
	gl_FragDepth = 1.0;
}