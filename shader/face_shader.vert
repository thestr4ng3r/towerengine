#version 130

uniform mat4 gl_TextureMatrix[gl_MaxTextureCoords];
uniform mat4 gl_ModelViewProjectionMatrix;
uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ModelViewMatrixInverse;

in vec2 TexCoord_a;
in vec3 Vertex_a;
in vec3 Normal_a;
in vec3 FaceNormal_a;

in vec4 TexBorder_a;
in unsigned int DiscardBorder_a;

in float BumpFactor_a;

uniform mat4 Transformation;

uniform vec3 LightPos;

uniform sampler2D HeightTex;
uniform vec3 HeightFactor;

out vec2 UVCoord;
out vec4 ShadowCoord;
out vec3 LightDir;
out vec3 CamPos;
out vec3 CamDir;
out vec3 Normal;
out vec3 FaceNormal;
out vec3 TangX;
out vec3 TangY;
out vec3 Pos;

out vec4 TexBorder;
out float DiscardBorder;

out float BumpFactor;

void main(void)
{
	vec3 normal = normalize(Normal_a * mat3(Transformation));
	vec3 tangx;// = normalize(TangX_a * mat3(Transformation));
	vec3 tangy;// = normalize(TangY_a * mat3(Transformation));

	if(dot(Normal_a, vec3(0.0, 1.0, 0.0)) > 0.5)
	{
		tangx = normalize(cross(Normal_a, vec3(0.0, 0.0, 1.0)));
		tangy = normalize(-cross(Normal_a, tangx));
	}
	else
	{
		tangx = normalize(cross(Normal_a, vec3(0.0, 1.0, 0.0)));
		tangy = normalize(-cross(Normal_a, tangx));
	}
		
	Normal = normal;
	TangX = tangx;
	TangY = tangy;
	
	vec3 shift = vec3(0.0, 0.0, 0.0);
	if(dot(HeightFactor, vec3(1.0, 1.0, 1.0)) > 0.001)
	{
		vec3 displacement = texture2D(HeightTex, TexCoord_a).rgb;
		displacement -= vec3(0.5, 0.5, 0.5);
		shift = tangx * displacement.x;
		shift += tangy * displacement.y;
		shift += normal * displacement.z;
		shift *= HeightFactor;
	}
	vec4 pos = vec4(Vertex_a.xyz + shift, 1.0) * Transformation;
	Pos = pos.xyz;
	UVCoord = TexCoord_a.xy;
	ShadowCoord = gl_TextureMatrix[5] * pos;
	gl_Position = gl_ModelViewProjectionMatrix * pos;
	vec3 camera_dir = (gl_ModelViewMatrix * pos).xyz;
	vec3 light_dir = LightPos - pos.xyz;
	mat3 normal_mat = mat3(tangx, tangy, normal);
	CamPos = normal_mat * camera_dir;
	vec4 camera_OS = gl_ModelViewMatrixInverse * vec4(0.0, 0.0, 0.0, 1.0);
	CamDir = normalize(camera_OS.xyz * pos.w - pos.xyz * camera_OS.w);
	LightDir.x = dot(tangx, light_dir);
	LightDir.y = dot(tangy, light_dir);
	LightDir.z = dot(normal, light_dir);	
	FaceNormal = FaceNormal_a;
	DiscardBorder = float(DiscardBorder_a);
}