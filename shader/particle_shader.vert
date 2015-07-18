#version 330

uniform mat4 modelview_matrix_uni;

in vec3 vertex_attr;
in float size_attr;
in float rotation_attr;
in vec4 color_attr;

in float texture_index_attr;

out float size_var;
out float rotation_var;
flat out vec4 color_var_g;
flat out float texture_index_var_g;

void main(void)
{
	size_var = size_attr;
	rotation_var = rotation_attr;
	color_var_g = color_attr;
	texture_index_var_g = texture_index_attr;
	 
	gl_Position = modelview_matrix_uni * vec4(vertex_attr, 1.0);
}