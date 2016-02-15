#version 330

uniform sampler2DArray texture_uni;

uniform vec3 lighting_normal_uni;
uniform vec3 face_normal_uni;

in vec2 uv_var;
flat in float texture_index_var_f;
flat in vec4 color_var_f;

out vec4 color_out;

layout (location = 0) out vec4 diffuse_out;
layout (location = 1) out vec4 normal_out;
layout (location = 2) out vec4 face_normal_out;
layout (location = 3) out vec4 specular_out;
layout (location = 4) out vec4 self_illumination_out;

void main(void)
{
	vec4 color = texture(texture_uni, vec3(uv_var, float(texture_index_var_f)));

	if(color.a < 0.5)
		discard;

	color_out = vec4(color_var_f.rgb * color.rgb, 1.0);
	normal_out = vec4(lighting_normal_uni * 0.5 + vec3(0.5), 1.0);
	face_normal_out = vec4(face_normal_uni * 0.5 + vec3(0.5), 1.0);
	specular_out = vec4(0.0, 0.0, 0.0, 1.0);
	self_illumination_out = vec4(0.0, 0.0, 0.0, 1.0);
}