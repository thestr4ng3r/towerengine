#include "towerengine.h"

int shader_enabled = 1;

CFaceShader face_shader;

void CFaceShader::Init(void)
{
	printf("Compiling Face Shader...\n");
	SetSource(face_shader_vert, face_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();
	glBindAttribLocationARB(program, vertex_attribute, "Vertex_a");
	glBindAttribLocationARB(program, normal_attribute, "Normal_a");
	glBindAttribLocationARB(program, face_normal_attribute, "FaceNormal_a");
	glBindAttribLocationARB(program, uvcoord_attribute, "TexCoord_a");
	LinkProgram();

	two_side_uniform = glGetUniformLocationARB(program, "TwoSide");

	diffuse_color_uniform = glGetUniformLocationARB(program, "DiffuseColor");
	diffuse_color2_uniform = glGetUniformLocationARB(program, "DiffuseColor2");
	specular_color_uniform = glGetUniformLocationARB(program, "SpecularColor");
	ambient_color_uniform = glGetUniformLocationARB(program, "AmbientColor");

	light_pos_uniform = glGetUniformLocationARB(program, "LightPos");
	light_color_uniform = glGetUniformLocationARB(program, "LightColor");
	
	shadow_map_uniform = glGetUniformLocationARB(program, "ShadowMap");
	shadow_enabled_uniform = glGetUniformLocationARB(program, "ShadowEnabled");
	shadow_pixel_uniform = glGetUniformLocationARB(program, "ShadowPixel");

	specular_size_uniform = glGetUniformLocationARB(program, "SpecularSize");

	diffuse_tex_uniform = glGetUniformLocationARB(program, "DiffuseTex");
	aeb_tex_uniform = glGetUniformLocationARB(program, "AEBTex");
	normal_tex_uniform = glGetUniformLocationARB(program, "NormalTex");
	height_tex_uniform = glGetUniformLocationARB(program, "HeightTex");
	specular_tex_uniform = glGetUniformLocationARB(program, "SpecularTex");

	tex_border_uniform = glGetUniformLocationARB(program, "TexBorder");
	discard_border_uniform = glGetUniformLocationARB(program, "DiscardBorder");

	height_factor_uniform = glGetUniformLocationARB(program, "HeightFactor");
	bump_factor_uniform = glGetUniformLocationARB(program, "BumpFactor");

	fog_color_uniform = glGetUniformLocationARB(program, "FogColor");
	fog_depth_uniform = glGetUniformLocationARB(program, "FogDepth");
	fog_dist_uniform = glGetUniformLocationARB(program, "FogDist");

	transformation_uniform = glGetUniformLocationARB(program, "Transformation");
	shader_mode_uniform = glGetUniformLocationARB(program, "ShaderMode");

	clip_uniform = glGetUniformLocationARB(program, "Clip");
	clip_dist_uniform = glGetUniformLocationARB(program, "ClipDist");

	ResetUniforms();
	UseNoShader();
}


void CFaceShader::SetMode(int e)
{
	if(shader_mode_uniform != -1)
		glUniform1i(shader_mode_uniform, e);
}

void CFaceShader::SetTransformation(float m[16])
{
	if(transformation_uniform != -1)
		glUniformMatrix4fvARB(transformation_uniform, 1, GL_FALSE, m);
}

void CFaceShader::SetDiffuseColor(CVector color)
{
	if(diffuse_color_uniform != -1)
		glUniform3fARB(diffuse_color_uniform, color.x, color.y, color.z);
}

void CFaceShader::SetDiffuseColor2(CVector color, float alpha)
{
	if(diffuse_color_uniform != -1)
		glUniform4fARB(diffuse_color2_uniform, color.x, color.y, color.z, alpha);
}

void CFaceShader::SetSpecularColor(CVector color)
{
	if(specular_color_uniform != -1)
		glUniform3fARB(specular_color_uniform, color.x, color.y, color.z);
}

void CFaceShader::SetAmbientColor(CVector color)
{
	if(ambient_color_uniform != -1)
		glUniform3fARB(ambient_color_uniform, color.x, color.y, color.z);
}

void CFaceShader::SetLight(CVector pos, CVector color)
{
	if(light_pos_uniform != -1)
		glUniform3fARB(light_pos_uniform, pos.x, pos.y, pos.z);
	if(light_color_uniform != -1)
		glUniform3fARB(light_color_uniform, color.x, color.y, color.z);
}

void CFaceShader::SetSpecular(float size)
{
	if(specular_size_uniform != -1)
		glUniform1f(specular_size_uniform, size);

}

void CFaceShader::SetTexture(GLuint diffuse, GLuint aeb, GLuint normal, GLuint height, GLuint specular)
{
	glUniform1iARB(diffuse_tex_uniform, 0);
	glUniform1iARB(aeb_tex_uniform, 1);
	glUniform1iARB(normal_tex_uniform, 2);
	glUniform1iARB(height_tex_uniform, 3);
	glUniform1iARB(specular_tex_uniform, 4);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, diffuse);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, aeb);
	glActiveTexture(GL_TEXTURE2_ARB);
	glBindTexture(GL_TEXTURE_2D, normal);
	glActiveTexture(GL_TEXTURE3_ARB);
	glBindTexture(GL_TEXTURE_2D, height);
	glActiveTexture(GL_TEXTURE4_ARB);
	glBindTexture(GL_TEXTURE_2D, specular);
}

void CFaceShader::SetShadow(int v, GLuint tex, CVector2 pixel)
{
	glUniform1iARB(shadow_map_uniform, 5);

	glActiveTexture(GL_TEXTURE5_ARB);
	glBindTexture(GL_TEXTURE_2D, tex);

	if(shadow_enabled_uniform != -1)
		glUniform1i(shadow_enabled_uniform, v ? 1 : 0);
	if(shadow_pixel_uniform != -1)
		glUniform2f(shadow_pixel_uniform, pixel.x, pixel.y);
}


void CFaceShader::SetTexCoord(CVector2 coord)
{
	glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, coord.v);
}

void CFaceShader::SetBorder(int e, CVector2 a, CVector2 b)
{
	CVector small, big;

	small.x = a.x < b.x ? a.x : b.x;
	small.y = a.y < b.y ? a.y : b.y;
	big.x = a.x > b.x ? a.x : b.x;
	big.y = a.y > b.y ? a.y : b.y;

	if(tex_border_uniform != -1)
		glUniform4f(tex_border_uniform, small.x, small.y, big.x, big.y);
	if(discard_border_uniform != -1)
		glUniform1i(discard_border_uniform, e ? 1 : 0);
}

void CFaceShader::SetVectors(CVector normal, CVector tangx, CVector tangy, CVector fnormal)
{
	if(normal.SquaredLen() < 0.5)
		printf("%f, %f, %f\n", pvec(normal));
	normal.AttrToGL(normal_attribute);
	//Vec(0.0, 1.0, 0.0).AttrToGL(ATTR_NORMAL);

	if(fnormal.SquaredLen() > 0.0)
		fnormal.AttrToGL(normal_attribute);
	else
		normal.AttrToGL(normal_attribute);
}

void CFaceShader::SetHeightFactor(CVector factor)
{
	if(height_factor_uniform != -1)
		glUniform3f(height_factor_uniform, factor.x, factor.y, factor.z);
}

void CFaceShader::SetBumpFactor(float f)
{
	if(bump_factor_uniform != -1)
		glUniform1f(bump_factor_uniform, f);
}

void CFaceShader::SetTwoSide(int v)
{
	if(two_side_uniform != -1)
		glUniform1i(two_side_uniform, v ? 1 : 0);
}

void CFaceShader::SetFog(CVector color, float depth, float dist)
{
	if(fog_color_uniform != -1)
		glUniform3f(fog_color_uniform, color.x, color.y, color.z);
	if(fog_depth_uniform != -1)
		glUniform1f(fog_depth_uniform, depth);
	if(fog_dist_uniform != -1)
		glUniform1f(fog_dist_uniform, dist);
}

void CFaceShader::SetClip(CVector c, float d)
{
	glUniform3f(clip_uniform, c.x, c.y, c.z);
	glUniform1f(clip_dist_uniform, d);
}

void CFaceShader::ResetUniforms(void)
{
	SetVectors(Vec(0.0, 0.0, 1.0), Vec(1.0, 0.0, 0.0), Vec(0.0, 1.0, 0.0)); 
	//SetDiffuseColor(Vec(1.0, 1.0, 1.0));
	SetSpecularColor(Vec(0.5, 0.5, 0.5));
	SetAmbientColor(Vec(0.3, 0.3, 0.3));
	SetLight(Vec(0.0, 0.0, 0.0), Vec(0.0, 0.0, 0.0));
	SetBorder(0, Vec(0.0, 0.0), Vec(0.0, 0.0));
	SetSpecular(32.0);
	SetFog(Vec(1.0, 1.0, 1.0), 0.0, 0.0);
	SetClip(Vec(0.0, 0.0, 0.0), 0.0);
	SetShadow(0, 0, Vec(1.0, 1.0));
}

