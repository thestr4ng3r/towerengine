#version 130

uniform mat4 gl_ModelViewProjectionMatrix;

in vec3 vertex_attr;
in vec3 vertex2_attr; // vertex of next keyframe

uniform float vertex_mix_uni;
uniform mat4 transformation_uni;

uniform vec3 light_dir_uni;
uniform vec2 clip_uni;
uniform vec3 cam_pos_uni;

out float moment1_var;

void main(void)
{
	vec3 vertex_pos = vertex_attr;
	if(vertex_mix_uni > 0.0)
		vertex_pos = vertex_pos * (1.0 - vertex_mix_uni) + vertex2_attr * vertex_mix_uni;
	vec4 pos = vec4(vertex_pos, 1.0) * transformation_uni;
	
	moment1_var = (dot(pos.xyz - cam_pos_uni, light_dir_uni) + clip_uni.x) / (clip_uni.y - clip_uni.x);
	
	gl_Position = gl_ModelViewProjectionMatrix * pos;
}