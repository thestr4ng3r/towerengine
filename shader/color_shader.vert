#version 330

uniform mat4 gl_ModelViewProjectionMatrix;

in vec3 vertex_attr;

uniform mat4 transformation_uni;

void main(void)
{
	vec4 pos = vec4(vertex_attr, 1.0) * transformation_uni;
	
	gl_Position = gl_ModelViewProjectionMatrix * pos;
}