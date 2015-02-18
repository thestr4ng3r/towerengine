#version 330

uniform mat4 gl_ModelViewProjectionMatrix;

in vec4 vertex_attr;

out vec2 uv_var;

void main(void)
{
	uv_var = vertex_attr.xy;
	gl_Position = gl_ModelViewProjectionMatrix * vertex_attr;
}