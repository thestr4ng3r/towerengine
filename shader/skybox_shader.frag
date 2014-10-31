#version 330

uniform samplerCube cube_map_uni;

in vec3 pos_var;

out vec4 gl_FragColor;

void main(void)
{
	vec3 color = texture(cube_map_uni, normalize(pos_var)).xyz;
	gl_FragColor = vec4(color, 1.0);
}