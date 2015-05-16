#version 330

uniform mat4 gl_ModelViewProjectionMatrix;

uniform mat4 transformation_uni;

in vec3 vertex_attr;

noperspective out vec2 uv_coord_var;
out vec3 pos_var;

void main(void)
{
	vec3 vertex_pos = (vec4(vertex_attr, 1.0) * transformation_uni).xyz;
	vec4 pos = gl_ModelViewProjectionMatrix * vec4(vertex_pos, 1.0);
	uv_coord_var = (pos.xy / pos.w) * 0.5 + vec2(0.5);
	pos_var = vertex_pos;
	gl_Position = pos;
}

