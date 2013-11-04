uniform samplerCube Texture;

varying vec3 Pos;

vec3 pos;

void main(void)
{
	vec3 color = textureCube(Texture, normalize(Pos)).xyz;
	gl_FragColor = vec4(color, 1.0);
}