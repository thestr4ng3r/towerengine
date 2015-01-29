#version 330

uniform sampler2D color_tex_uni;
uniform vec2 tex_pixel_uni;

uniform bool fxaa_enabled_uni;

in vec2 uv_coord_var;

out vec4 gl_FragColor;



#define FXAA_EDGE_THRESHOLD_MIN	(1.0 / 16.0)
#define FXAA_EDGE_THRESHOLD		(1.0 / 8.0)
#define FXAA_SPAN_MAX			8.0
#define FXAA_REDUCE_MUL			(1.0 / 8.0)
#define FXAA_REDUCE_MIN			(1.0 / 128.0)

vec3 FXAA(vec3 rgb_m)
{
	vec3 rgb_nw = texture(color_tex_uni, uv_coord_var + vec2(-1.0, 1.0) * tex_pixel_uni).rgb;
	vec3 rgb_ne = texture(color_tex_uni, uv_coord_var + vec2(1.0, 1.0) * tex_pixel_uni).rgb;
	vec3 rgb_sw = texture(color_tex_uni, uv_coord_var + vec2(-1.0, -1.0) * tex_pixel_uni).rgb;
	vec3 rgb_se = texture(color_tex_uni, uv_coord_var + vec2(1.0, -1.0) * tex_pixel_uni).rgb;
	
	vec3 luma = vec3(0.299, 0.587, 0.114);
	
	float luma_nw = dot(rgb_nw, luma);
	float luma_ne = dot(rgb_ne, luma);
	float luma_sw = dot(rgb_sw, luma);
	float luma_se = dot(rgb_se, luma);
	float luma_m = dot(rgb_m, luma);
	
	float luma_min = min(luma_m, min(min(luma_nw, luma_ne), min(luma_sw, luma_se)));
	float luma_max = max(luma_m, max(max(luma_nw, luma_ne), max(luma_sw, luma_se)));
	
	if(luma_max - luma_min < max(FXAA_EDGE_THRESHOLD_MIN, luma_max * FXAA_EDGE_THRESHOLD))
		return rgb_m;
		

	vec2 dir;
	dir.x = -((luma_nw + luma_ne) - (luma_sw + luma_se));
	dir.y = ((luma_nw + luma_sw) - (luma_ne + luma_se));
	
	float dir_reduce = max((luma_nw + luma_ne + luma_sw + luma_se) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	float rcp_dir_min = 1.0/(min(abs(dir.x), abs(dir.y)) + dir_reduce);
	
	dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
			max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcp_dir_min)) * tex_pixel_uni;
			
			
	vec3 rgb_a = (1.0/2.0) * (
			texture2D(color_tex_uni, uv_coord_var + dir * (1.0/3.0 - 0.5)).xyz +
			texture2D(color_tex_uni, uv_coord_var + dir * (2.0/3.0 - 0.5)).xyz);
	vec3 rgb_b = rgb_a * (1.0/2.0) + (1.0/4.0) * (
			texture2D(color_tex_uni, uv_coord_var + dir * (0.0/3.0 - 0.5)).xyz +
			texture2D(color_tex_uni, uv_coord_var + dir * (3.0/3.0 - 0.5)).xyz);
	float luma_b = dot(rgb_b, luma);
	
	
	if((luma_b < luma_min) || (luma_b > luma_max))
		return rgb_a;
	else
		return rgb_b;
}

void main(void)
{
	vec4 src_color = texture(color_tex_uni, uv_coord_var);
	vec3 color = vec3(0.0, 0.0, 0.0);
	
	if(fxaa_enabled_uni)
		color = FXAA(src_color.rgb);
	else
		color = src_color.rgb;	
		
	
	gl_FragColor = vec4(color, src_color.a);
}