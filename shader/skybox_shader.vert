#version 330

uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ProjectionMatrix;

uniform mat4 gl_ModelViewProjectionMatrix;

in vec3 vertex_attr;

out vec3 pos_var;

void main(void)
{
	pos_var = vertex_attr;
	vec4 p = gl_ModelViewMatrix * vec4(vertex_attr, 0.0);
	gl_Position = gl_ProjectionMatrix * vec4(p.xyz, 1.0);
}