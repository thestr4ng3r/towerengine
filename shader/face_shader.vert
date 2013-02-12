#version 130

uniform mat4 gl_TextureMatrix[gl_MaxTextureCoords];
uniform mat4 gl_ModelViewProjectionMatrix;
uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ModelViewMatrixInverse;

in vec3 vertex_attr;
in vec2 uv_attr;
in vec3 normal_attr;
in vec3 tang_attr;
in vec3 bitang_attr;

uniform mat4 transformation_uni;

out vec3 pos_var;
out vec3 normal_var;
out vec3 tang_var;
out vec3 bitang_var;
out vec2 uv_var;

out vec3 cam_pos_var;
out vec4 shadow_coord_var;

void main(void)
{
	vec4 pos = vec4(vertex_attr, 1.0) * transformation_uni;
	pos_var = pos.xyz;
	normal_var = normalize(normal_attr * mat3(transformation_uni));
	tang_var = normalize(tang_attr * mat3(transformation_uni));
	bitang_var = normalize(bitang_attr * mat3(transformation_uni));
	uv_var = uv_attr;
	
	cam_pos_var = gl_ModelViewMatrixInverse[3].xyz;
	shadow_coord_var = gl_TextureMatrix[5] * pos;
	
	gl_Position = gl_ModelViewProjectionMatrix * pos;
}