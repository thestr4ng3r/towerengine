#include "towerengine.h"

int shader_enabled = 1;

const GLint CFaceShader::vertex_attribute;
const GLint CFaceShader::normal_attribute;
const GLint CFaceShader::tang_attribute;
const GLint CFaceShader::bitang_attribute;
const GLint CFaceShader::uvcoord_attribute;

void CFaceShader::Init(void)
{
	printf("Compiling Face Shader...\n");
	SetSource(face_shader_vert, face_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();
	glBindAttribLocationARB(program, vertex_attribute, "vertex_attr");
	glBindAttribLocationARB(program, vertex2_attribute, "vertex2_attr");
	glBindAttribLocationARB(program, normal_attribute, "normal_attr");
	glBindAttribLocationARB(program, tang_attribute, "tang_attr");
	glBindAttribLocationARB(program, bitang_attribute, "bitang_attr");
	glBindAttribLocationARB(program, uvcoord_attribute, "uv_attr");
	LinkProgram();

	two_side_uniform = glGetUniformLocationARB(program, "two_side_uni");

	vertex_mix_uniform = glGetUniformLocationARB(program, "vertex_mix_uni");

	diffuse_color_uniform = glGetUniformLocationARB(program, "diffuse_color_uni");
	diffuse_color2_uniform = glGetUniformLocationARB(program, "diffuse_color2_uni");
	specular_color_uniform = glGetUniformLocationARB(program, "specular_color_uni");
	ambient_uniform = glGetUniformLocationARB(program, "ambient_uni");

	point_light_count_uniform = glGetUniformLocationARB(program, "point_light_count_uni");
	point_light_pos_uniform = glGetUniformLocationARB(program, "point_light_pos_uni");
	point_light_color_uniform = glGetUniformLocationARB(program, "point_light_color_uni");
	point_light_distance_uniform = glGetUniformLocationARB(program, "point_light_distance_uni");
	point_light_shadow_map_uniform = glGetUniformLocationARB(program, "point_light_shadow_map_uni");
	point_light_shadow_enabled_uniform = glGetUniformLocationARB(program, "point_light_shadow_enabled_uni");
	point_light_shadow_near_clip_uniform = glGetUniformLocationARB(program, "point_light_shadow_near_clip_uni");

	directional_light_count_uniform = glGetUniformLocationARB(program, "directional_light_count_uni");
	directional_light_dir_uniform = glGetUniformLocationARB(program, "directional_light_dir_uni");
	directional_light_color_uniform = glGetUniformLocationARB(program, "directional_light_color_uni");

	light_ambient_color_uniform = glGetUniformLocationARB(program, "light_ambient_color_uni");

	specular_size_uniform = glGetUniformLocationARB(program, "specular_size_uni");

	diffuse_tex_uniform = glGetUniformLocationARB(program, "diffuse_tex_uni");
	normal_tex_uniform = glGetUniformLocationARB(program, "normal_tex_uni");
	height_tex_uniform = glGetUniformLocationARB(program, "height_tex_uni");
	specular_tex_uniform = glGetUniformLocationARB(program, "specular_tex_uni");

	diffuse_tex_enabled_uniform = glGetUniformLocationARB(program, "diffuse_tex_enabled_uni");
	normal_tex_enabled_uniform = glGetUniformLocationARB(program, "normal_tex_enabled_uni");
	height_tex_enabled_uniform = glGetUniformLocationARB(program, "height_tex_enabled_uni");
	specular_tex_enabled_uniform = glGetUniformLocationARB(program, "specular_tex_enabled_uni");

	//tex_border_uniform = glGetUniformLocationARB(program, "TexBorder");
	//discard_border_uniform = glGetUniformLocationARB(program, "DiscardBorder");

	//height_factor_uniform = glGetUniformLocationARB(program, "height_factor_uni");
	//bump_factor_uniform = glGetUniformLocationARB(program, "BumpFactor");

	transformation_uniform = glGetUniformLocationARB(program, "transformation_uni");
	shader_mode_uniform = glGetUniformLocationARB(program, "shader_mode_uni");

	clip_uniform = glGetUniformLocationARB(program, "clip_vec_uni");
	clip_dist_uniform = glGetUniformLocationARB(program, "clib_dist_uni");

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
	if(diffuse_color2_uniform != -1)
		glUniform4fARB(diffuse_color2_uniform, color.x, color.y, color.z, alpha);
}

void CFaceShader::SetSpecularColor(CVector color)
{
	if(specular_color_uniform != -1)
		glUniform3fARB(specular_color_uniform, color.x, color.y, color.z);
}

void CFaceShader::SetAmbient(float ambient)
{
	if(ambient_uniform != -1)
		glUniform1fARB(ambient_uniform, ambient);
}

void CFaceShader::SetPointLights(int count, float *pos, float *color, float *dist, int *shadow_enabled, GLuint *shadow_maps, float *shadow_near_clips)
{
	int i;
	count = min(count, max_point_lights);
	glUniform1iARB(point_light_count_uniform, count);
	glUniform3fvARB(point_light_pos_uniform, count, pos);
	glUniform3fvARB(point_light_color_uniform, count, color);
	glUniform1fvARB(point_light_distance_uniform, count, dist);

	glUniform1ivARB(point_light_shadow_enabled_uniform, count, shadow_enabled);
	int *tex_units = new int[max_point_lights];
	for(i=0; i<count; i++)
	{
		tex_units[i] = point_light_shadow_tex_first_unit + i;
		glActiveTextureARB(GL_TEXTURE0 + tex_units[i]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_maps[i]);
	}
	for(i=count; i<max_point_lights; i++)
		tex_units[i] = point_light_shadow_tex_first_unit + i;

	glUniform1ivARB(point_light_shadow_map_uniform, max_point_lights, tex_units);
	glUniform1fvARB(point_light_shadow_near_clip_uniform, count, shadow_near_clips);
}

void CFaceShader::SetDirectionalLights(int count, float *dir, float *color)
{
	count = min(count, max_directional_lights);
	glUniform1iARB(directional_light_count_uniform, count);
	glUniform3fvARB(directional_light_dir_uniform, count, dir);
	glUniform3fvARB(directional_light_color_uniform, count, color);
}

void CFaceShader::SetLightAmbientColor(CVector color)
{
	if(light_ambient_color_uniform != -1)
		glUniform3fARB(light_ambient_color_uniform, color.x, color.y, color.z);
}

void CFaceShader::SetSpecular(float size)
{
	if(specular_size_uniform != -1)
		glUniform1f(specular_size_uniform, size);

}


void CFaceShader::SetDiffuseTexture(bool enabled, GLuint tex)
{
	glUniform1iARB(diffuse_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1iARB(diffuse_tex_uniform, diffuse_tex_unit);
		glActiveTexture(GL_TEXTURE0 + diffuse_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}

void CFaceShader::SetSpecularTexture(bool enabled, GLuint tex)
{
	glUniform1iARB(specular_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1iARB(specular_tex_uniform, specular_tex_unit);
		glActiveTexture(GL_TEXTURE0 + specular_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

}

void CFaceShader::SetNormalTexture(bool enabled, GLuint tex)
{
	glUniform1iARB(normal_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1iARB(normal_tex_uniform, normal_tex_unit);
		glActiveTexture(GL_TEXTURE0 + normal_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

}

void CFaceShader::SetHeightTexture(bool enabled, GLuint tex)
{
	glUniform1iARB(height_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1iARB(height_tex_uniform, height_tex_unit);
		glActiveTexture(GL_TEXTURE0 + height_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
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

void CFaceShader::SetVertexMix(float m)
{
	glUniform1f(vertex_mix_uniform, m);
}

void CFaceShader::ResetUniforms(void)
{
	SetVectors(Vec(0.0, 0.0, 1.0), Vec(1.0, 0.0, 0.0), Vec(0.0, 1.0, 0.0)); 
	SetDiffuseColor(Vec(1.0, 1.0, 1.0));
	SetSpecularColor(Vec(0.5, 0.5, 0.5));
	SetAmbient(1.0);
	SetPointLights(0, 0, 0, 0, 0, 0, 0);
	SetLightAmbientColor(Vec(0.0, 0.0, 0.0));
	SetBorder(0, Vec(0.0, 0.0), Vec(0.0, 0.0));
	SetSpecular(32.0);
	SetFog(Vec(1.0, 1.0, 1.0), 0.0, 0.0);
	SetClip(Vec(0.0, 0.0, 0.0), 0.0);
	SetVertexMix(0.0);
	SetDiffuseTexture(false);
	SetSpecularTexture(false);
	SetNormalTexture(false);
	SetHeightTexture(false);
}

