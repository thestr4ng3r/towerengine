#version 330

uniform int screen_size_uni[2];

uniform int sprite_coords_uni[4];

in vec2 vertex_attr;

out vec2 uv_var;

void main(void)
{
	vec2 pos = vec2(sprite_coords_uni[0], sprite_coords_uni[1]);
	pos += vertex_attr * vec2(sprite_coords_uni[2], sprite_coords_uni[3]);
	pos /= vec2(screen_size_uni[0], screen_size_uni[1]);
	pos = pos * 2.0 - 1.0;
	pos.y *= -1.0;
	
	uv_var = vec2(vertex_attr.x, 1.0 - vertex_attr.y);
	
	gl_Position = vec4(pos, 0.0, 1.0);
}