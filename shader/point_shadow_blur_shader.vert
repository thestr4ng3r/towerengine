#version 130

uniform mat4 gl_ModelViewProjectionMatrix;

in vec4 gl_Vertex;

out vec3 pos_var;

void main(void)
{
	pos_var = gl_Vertex.xyz;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
