#version 330

uniform vec3 reflection_color_uni;
uniform samplerCube cube_map_uni;

uniform vec3 cam_pos_uni;

uniform sampler2D normal_tex_uni;

noperspective in vec2 uv_coord_var;
in vec3 pos_var;

out vec4 gl_FragColor;

void main(void)
{
	vec3 normal = texture(normal_tex_uni, uv_coord_var).xyz * 2.0 - vec3(1.0);
	vec3 reflected = reflect(pos_var - cam_pos_uni, normal);
	vec3 color = texture(cube_map_uni, reflected).rgb * reflection_color_uni;

	gl_FragColor = vec4(color, 1.0);
}