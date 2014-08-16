#version 130

uniform mat4 gl_ModelViewProjectionMatrix;

in vec4 gl_Vertex;
in vec4 gl_MultiTexCoord0;

out vec2 uv_coord_var;

void main(void)
{
	uv_coord_var = gl_MultiTexCoord0.xy;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
