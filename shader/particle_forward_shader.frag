#version 330

uniform sampler2DArray texture_uni;

in vec2 uv_var;
flat in float texture_index_var_f;
flat in vec4 color_var_f;

out vec4 color_out;

void main(void)
{		
	color_out = color_var_f * texture(texture_uni, vec3(uv_var, float(texture_index_var_f)));
}