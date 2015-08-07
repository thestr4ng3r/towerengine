#version 330

uniform vec4 color_uni;

uniform bool tex_enabled_uni;

uniform sampler2D tex_uni;

in vec2 uv_var;

out vec4 color_out;

void main(void)
{
	if(!gl_FrontFacing)
		discard;
	
	vec4 color = color_uni;
	
	if(tex_enabled_uni)
		color *= texture(tex_uni, uv_var).rgba;
		
	color_out = color;
}