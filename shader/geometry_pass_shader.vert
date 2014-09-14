#version 330


uniform mat4 gl_ModelViewProjectionMatrix;
uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ModelViewMatrixInverse;

in vec3 vertex_attr;
in vec3 vertex2_attr; // vertex of next keyframe
in vec2 uv_attr;
in vec3 normal_attr;
in vec3 tang_attr;
in vec3 bitang_attr;

uniform float vertex_mix_uni;
uniform mat4 transformation_uni;

out vec3 pos_var;
out vec3 normal_var;
out vec3 tang_var;
out vec3 bitang_var;
out vec2 uv_var;


void main(void)
{
	vec3 vertex_pos = vertex_attr;
	if(vertex_mix_uni > 0.0)
		vertex_pos = vertex_pos * (1.0 - vertex_mix_uni) + vertex2_attr * vertex_mix_uni;
	vec4 pos = vec4(vertex_pos, 1.0) * transformation_uni;
	pos_var = pos.xyz;
	normal_var = normalize(normal_attr * mat3(transformation_uni));
	tang_var = normalize(tang_attr * mat3(transformation_uni));
	bitang_var = normalize(bitang_attr * mat3(transformation_uni));
	uv_var = uv_attr;
	
	gl_Position = gl_ModelViewProjectionMatrix * pos;
}