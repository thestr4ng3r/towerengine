#version 330

uniform mat4 projection_matrix_uni;

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in float size_var[];
in float rotation_var[];
flat in float texture_index_var_g[];
flat in vec4 color_var_g[];

out vec2 uv_var;
flat out float texture_index_var_f;
flat out vec4 color_var_f;

void main(void)
{
	float size = size_var[0];

	vec2 up = vec2(sin(rotation_var[0]), cos(rotation_var[0]));
	vec2 right = vec2(up.y, -up.x);

	gl_Position = projection_matrix_uni * (gl_in[0].gl_Position + vec4((-right + up) * size, 0.0, 0.0));
	uv_var = vec2(0.0, 1.0);
	texture_index_var_f = texture_index_var_g[0];
	color_var_f = color_var_g[0];
	EmitVertex();
	
	gl_Position = projection_matrix_uni * (gl_in[0].gl_Position + vec4((-right - up) * size, 0.0, 0.0));
	uv_var = vec2(0.0, 0.0);
	texture_index_var_f = texture_index_var_g[0];
	color_var_f = color_var_g[0];
	EmitVertex();
	
	gl_Position = projection_matrix_uni * (gl_in[0].gl_Position + vec4((right + up) * size, 0.0, 0.0));
	uv_var = vec2(1.0, 1.0);
	texture_index_var_f = texture_index_var_g[0];
	color_var_f = color_var_g[0];
	EmitVertex();
	
	gl_Position = projection_matrix_uni * (gl_in[0].gl_Position + vec4((right - up) * size, 0.0, 0.0));
	uv_var = vec2(1.0, 0.0);
	texture_index_var_f = texture_index_var_g[0];
	color_var_f = color_var_g[0];
	EmitVertex();
	
	EndPrimitive();
}