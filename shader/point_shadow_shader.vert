#version 130

uniform mat4 gl_ModelViewProjectionMatrix;
uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ModelViewMatrixInverse;

in vec3 vertex_attr;
in vec3 vertex2_attr; // vertex of next keyframe

uniform float vertex_mix_uni;
uniform mat4 transformation_uni;

uniform vec3 light_pos_uni;
uniform float light_dist_uni;

out float moment1_var;

void main(void)
{
	vec3 vertex_pos = vertex_attr;
	if(vertex_mix_uni > 0.0)
		vertex_pos = vertex_pos * (1.0 - vertex_mix_uni) + vertex2_attr * vertex_mix_uni;
	vec4 pos = vec4(vertex_pos, 1.0) * transformation_uni;
	
	vec3 v = pos.xyz - light_pos_uni;
	float d = v.x*v.x + v.y*v.y + v.z*v.z;
			
	moment1_var = sqrt(d) / light_dist_uni; // TODO: to vertex shader
	
	gl_Position = gl_ModelViewProjectionMatrix * pos;
}