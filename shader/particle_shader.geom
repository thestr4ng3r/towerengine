#version 330

uniform mat4 projection_matrix_uni;

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in float size_var[];

out vec2 uv_var;

void main(void)
{
	float size = size_var[0];

	gl_Position = projection_matrix_uni * (gl_in[0].gl_Position + vec4(-1.0, 1.0, 0.0, 0.0) * size);
	uv_var = vec2(0.0, 1.0);
	EmitVertex();
	
	gl_Position = projection_matrix_uni * (gl_in[0].gl_Position + vec4(-1.0, -1.0, 0.0, 0.0) * size);
	uv_var = vec2(0.0, 0.0);
	EmitVertex();
	
	gl_Position = projection_matrix_uni * (gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0) * size);
	uv_var = vec2(1.0, 1.0);
	EmitVertex();
	
	gl_Position = projection_matrix_uni * (gl_in[0].gl_Position + vec4(1.0, -1.0, 0.0, 0.0) * size);
	uv_var = vec2(1.0, 0.0);
	EmitVertex();
	
	EndPrimitive();
}