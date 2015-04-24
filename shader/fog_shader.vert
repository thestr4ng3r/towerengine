#version 330

uniform mat4 gl_ModelViewProjectionMatrix;

in vec4 gl_Vertex;

out vec2 uv_coord_var;

void main(void)
{
	uv_coord_var = gl_Vertex.xy;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

