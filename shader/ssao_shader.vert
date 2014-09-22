#version 330

uniform mat4 gl_ModelViewProjectionMatrix;

in vec4 gl_Vertex;

out vec2 uv_var;
noperspective out vec3 view_ray_var;

void main(void)
{
	uv_var = gl_Vertex.xy;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}