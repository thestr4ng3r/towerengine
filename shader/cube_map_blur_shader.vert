#version 330

in vec2 vertex_attr;

uniform vec3 blur_dir_uni;

out vec3 coords_var[6];
out vec3 blur_dir_var[6];

void main(void)
{
	coords_var[0] = vec3(1.0, -vertex_attr.y, -vertex_attr.x);
	coords_var[1] = vec3(-1.0, -vertex_attr.y, vertex_attr.x);
	
	coords_var[2] = vec3(vertex_attr.x, 1.0, vertex_attr.y);
	coords_var[3] = vec3(vertex_attr.x, -1.0, -vertex_attr.y);
	
	coords_var[4] = vec3(vertex_attr.x, -vertex_attr.y, 1.0);
	coords_var[5] = vec3(-vertex_attr.x, -vertex_attr.y, -1.0);
	
	blur_dir_var[0] = vec3(0.0, blur_dir_uni.y, blur_dir_uni.x);
	blur_dir_var[1] = vec3(0.0, blur_dir_uni.y, -blur_dir_uni.x);
	
	blur_dir_var[2] = vec3(blur_dir_uni.x, 0.0, blur_dir_uni.y);
	blur_dir_var[3] = vec3(blur_dir_uni.x, 0.0, -blur_dir_uni.y);
	
	blur_dir_var[4] = vec3(-blur_dir_uni.x, blur_dir_uni.y, 0.0);
	blur_dir_var[5] = vec3(blur_dir_uni.x, blur_dir_uni.y, 0.0);
		
	gl_Position = vec4(vertex_attr, -1.0, 1.0);
}
