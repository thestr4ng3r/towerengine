#version 330

#define SSAO_BLUR_SIZE 4

uniform sampler2D ssao_tex_uni;
uniform sampler2D diffuse_tex_uni;

in vec2 uv_coord_var;

out vec4 gl_FragColor;

void main(void)
{
	if(texture(diffuse_tex_uni, uv_coord_var).a == 0.0)
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
 
	gl_FragColor = vec4(vec3(result / float(SSAO_BLUR_SIZE * SSAO_BLUR_SIZE)), 1.0);
}