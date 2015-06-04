#version 330

uniform mat4 modelview_projection_matrix_uni;

uniform mat4 transformation_uni;

in vec3 vertex_attr;
in vec4 color_attr;

out vec4 color_var;

void main(void)
{
	color_var = color_attr;
	
	vec4 pos = vec4(vertex_attr, 1.0) * transformation_uni;
		
	gl_Position = modelview_projection_matrix_uni * pos;
}