#version 130

in vec2 UVCoord;
in vec4 ShadowCoord;
in vec3 LightDir;
in vec3 CamPos;
in vec3 CamDir;
in vec3 Pos;
in vec3 Normal;
in vec3 FaceNormal;
in vec3 TangX;
in vec3 TangY;

out vec4 gl_FragColor;

uniform bool ShaderMode;

uniform bool TwoSide;

uniform vec3 LightColor;
uniform sampler2DShadow ShadowMap;

uniform vec3 DiffuseColor;
uniform vec4 DiffuseColor2;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularSize;

uniform sampler2D DiffuseTex;
uniform sampler2D AEBTex;
uniform sampler2D NormalTex;
uniform sampler2D SpecularTex;

in vec4 TexBorder;
in float DiscardBorder;

uniform vec3 Clip;
uniform float ClipDist;

in float BumpFactor;

uniform vec3 FogColor;
uniform float FogDepth;
uniform float FogDist;

vec3 normal;
vec3 light_dir;
vec3 cam_dir;
vec3 cam_dir_n;

uniform vec2 ShadowPixel;

float CalculateSpecularLight(in vec3 light_dir)
{
	return max(dot(normalize(reflect(-light_dir, normal)), cam_dir_n), 0.0);
}

vec2 CalculateTexCoord(out vec2 _dx, out vec2 _dy, in sampler2D height_map, in int channel, in vec2 flat_coord, in float max_depth, in vec3 cam_dir)
{
	vec2 dx = dFdx(flat_coord);
	vec2 dy = dFdy(flat_coord);
	_dx = dx;
	_dy = dy;
	
	if(max_depth <= 0.0)
		return flat_coord;
	if(texture2D(height_map, flat_coord)[channel] == 1.0)
		return flat_coord;
	
	float max_offset = length(cam_dir.xy) / cam_dir.z;
	max_offset *= max_depth;
	
	vec2 offset_v = normalize(cam_dir.xy);
	offset_v *= max_offset;
	
	int num_samples = int(mix(64.0, 8.0, dot(vec3(0.0, 0.0, 1.0), cam_dir_n)));
	float step_size = 1.0 / float(num_samples);
	
	vec2 offset_step = offset_v * step_size;
	vec2 current_offset = vec2(0.0, 0.0);
	vec2 last_offset = vec2(0.0, 0.0);
	vec2 final_offset = vec2(0.0, 0.0);
	
	vec4 current_sample_v = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 last_sample = vec4(0.0, 0.0, 0.0, 0.0);
	
	float step_height = 1.0;
	int current_sample = 0;
	
	while(current_sample < num_samples)
	{
		current_sample_v = textureGrad(height_map, flat_coord + current_offset, dx, dy);
		if(current_sample_v[channel] > step_height)
		{
			break;
		}
		else
		{
			current_sample++;
			step_height -= step_size;
			last_offset = current_offset;
			current_offset += offset_step;
			last_sample = current_sample_v;
		}
	}
	float ua = (last_sample[channel] - (step_height + step_size))
			/ (step_size + current_sample_v[channel] - last_sample[channel]);
	final_offset = last_offset + ua * offset_step;
	return flat_coord + final_offset;
}

vec4 ShadowCoordO(vec2 offset)
{
	return ShadowCoord + vec4(offset.x * ShadowPixel.x * ShadowCoord.w,
			offset.y * ShadowPixel.y * ShadowCoord.w, -0.0005, 0.0);
}

vec4 ShadowCoordW(vec2 offset)
{
	return ShadowCoordO(offset) / ShadowCoord.w;
}

float ShadowLookup(vec2 offset)
{
	return textureProj(ShadowMap, ShadowCoordO(offset));
}

void main(void)
{
	if(!gl_FrontFacing && !TwoSide)
		discard;
	
	if(Clip != vec3(0.0, 0.0, 0.0))
	{
		vec3 clip = Clip * ClipDist;
		clip = Pos - clip;
		if(dot(clip, Clip) >= 0.0)
			discard;
	}
	
	if(!ShaderMode)
	{
		gl_FragColor = vec4(1.0);
		return;
	}
	
	int i;
	light_dir = normalize(LightDir);
	mat3 normal_mat = mat3(TangX, TangY, FaceNormal);
	cam_dir = CamDir * normal_mat;
	cam_dir_n = normalize(cam_dir);
	vec3 aeb = texture2D(AEBTex, UVCoord).rgb;
	float alpha = aeb.r;
	if(alpha == 0.0)
	{
		gl_FragColor = vec4(0.0);
		return;
	}
	float exponent_factor = aeb.g;
	float height = aeb.b;
	vec2 dx, dy;
	vec2 tex_coord = CalculateTexCoord(dx, dy, AEBTex, 2, UVCoord, BumpFactor, vec3(-cam_dir.xy, cam_dir.z));
	//if(DiscardBorder > 0.5 && (tex_coord.x < TexBorder.x || tex_coord.y < TexBorder.y || tex_coord.x > TexBorder.z || tex_coord.y > TexBorder.w))
	//	discard;
	vec3 color = DiffuseColor * textureGrad(DiffuseTex, tex_coord, dx, dy).rgb;
	vec3 frag_color = AmbientColor * color;
	normal = normalize(normalize(texture2D(NormalTex, tex_coord).rgb - 0.5) + (Normal - FaceNormal));
	normal.x = -normal.x;
	
	float shadow = 1.0;
	float sr = 1.5;
	int shadow_enabled = 1;
	if(ShadowCoordW(vec2(sr, 0.0)).x > 1.0 || ShadowCoordW(vec2(0.0, sr)).y > 1.0 ||
			ShadowCoordW(vec2(sr, 0.0)).x < 0.0 || ShadowCoordW(vec2(0.0, sr)).y < 0.0)
		shadow_enabled = 0;
	if(ShadowCoord.z / ShadowCoord.w > 1.0)
		shadow_enabled = 0;
	
	if(shadow_enabled == 1)
	{
		float x,y;
		shadow = 0.0;
		for (y=-sr; y<=sr; y+=(sr*2.0 + 1.0)/4.0)
			for (x = -sr ; x <=sr ; x+=(sr*2.0 + 1.0)/4.0)
				shadow += ShadowLookup(vec2(x,y));
		shadow /= 16.0;
	}
	
	frag_color += max(0.0, dot(light_dir, normal)) * color * LightColor * shadow;
	
	vec3 specular_color = SpecularColor * texture2D(SpecularTex, tex_coord).rgb * LightColor;
	float specular_intensity = CalculateSpecularLight(light_dir);
	frag_color += max(vec3(0.0, 0.0, 0.0), specular_color * pow(specular_intensity, SpecularSize) * shadow);
	
	if(FogDepth > 0.0)
	{
		float fog = clamp((abs(CamPos.z) - FogDist) * FogDepth, 0.0, 1.0);
		frag_color = mix(frag_color, FogColor, fog);
	}
	
	frag_color *= DiffuseColor2.rgb;
	alpha *= DiffuseColor2.a;
		
	gl_FragColor = vec4(frag_color, alpha);
}