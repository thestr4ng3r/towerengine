#version 330

in vec2 vertex_attr;

void main(void)
{	
	gl_Position = vec4(vertex_attr, 0.0, 1.0);
}
