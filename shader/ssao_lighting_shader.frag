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
		
	vec2 texel_size = 1.0 / textureSize(ssao_tex_uni, 0);
	float result = 0.0;
	
	vec2 hlim = vec2(float(SSAO_BLUR_SIZE) * 0.5 - 0.5);
	
	for(int i=0; i<SSAO_BLUR_SIZE; i++)
	{
		for(int j=0; j<SSAO_BLUR_SIZE; j++)
		{
			vec2 offset = (hlim + vec2(float(i), float(j))) * texel_size;
			result += texture(ssao_tex_uni, uv_coord_var + offset).r;
		}
	}
	
	float occlusion = result / float(SSAO_BLUR_SIZE * SSAO_BLUR_SIZE);
		
	vec3 color = light_ambient_color_uni * diffuse.rgb * occlusion;
	
	gl_FragColor = vec4(color, 1.0);
}