#version 130

uniform mat4 gl_ModelViewProjectionMatrix;

in vec4 gl_Vertex;

uniform vec3 blur_dir_uni;

out vec3 coords_var[6];
out vec3 blur_dir_var[6];

void main(void)
{
	coords_var[0] = vec3(1.0, -gl_Vertex.y, -gl_Vertex.x);
	coords_var[1] = vec3(-1.0, -gl_Vertex.y, gl_Vertex.x);
	
	coords_var[2] = vec3(gl_Vertex.x, 1.0, gl_Vertex.y);
	coords_var[3] = vec3(gl_Vertex.x, -1.0, -gl_Vertex.y);
	
	coords_var[4] = vec3(gl_Vertex.x, -gl_Vertex.y, 1.0);
	coords_var[5] = vec3(-gl_Vertex.x, -gl_Vertex.y, -1.0);
	
	blur_dir_var[0] = vec3(0.0, blur_dir_uni.y, blur_dir_uni.x);
	blur_dir_var[1] = vec3(0.0, blur_dir_uni.y, -blur_dir_uni.x);
	
	blur_dir_var[2] = vec3(blur_dir_uni.x, 0.0, blur_dir_uni.y);
	blur_dir_var[3] = vec3(blur_dir_uni.x, 0.0, -blur_dir_uni.y);
	
	blur_dir_var[4] = vec3(-blur_dir_uni.x, blur_dir_uni.y, 0.0);
	blur_dir_var[5] = vec3(blur_dir_uni.x, blur_dir_uni.y, 0.0);
		
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
