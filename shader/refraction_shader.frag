#version 330

uniform vec3 color_uni;

uniform bool color_tex_enabled_uni;
uniform sampler2D color_tex_uni;

uniform sampler2D normal_tex_uni;

uniform sampler2D screen_tex_uni;

in vec2 uv_var;
noperspective in vec3 screen_uv_var; 

out vec4 color_out;

void main(void)
{
	if(!gl_FrontFacing)
		discard;
	
	vec4 color = vec4(color_uni, 1.0);
	
	if(color_tex_enabled_uni)
		color *= texture(color_tex_uni, uv_var).rgba;
	
	vec3 normal = texture(normal_tex_uni, uv_var).xyz * 2.0 - vec3(1.0);
	normal = normalize(normal);
	
	color.rgb *= texture(screen_tex_uni, screen_uv_var.xy + (normal.xy / screen_uv_var.z) * 0.01).rgb;
	
	color_out = color;
}