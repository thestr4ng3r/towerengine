#version 330

#define SSAO_BLUR_SIZE 4

uniform vec3 light_ambient_color_uni;

uniform sampler2D ssao_tex_uni;
uniform sampler2D diffuse_tex_uni;

in vec2 uv_coord_var;

out vec4 gl_FragColor;


void main(void)
{
	vec4 diffuse = texture2D(diffuse_tex_uni, uv_coord_var).rgba;

	if(diffuse.a == 0.0)
		discard;
		
	vec3 color = light_ambient_color_uni * diffuse.rgb; // ambient
	
	gl_FragColor = vec4(color, 1.0);
}