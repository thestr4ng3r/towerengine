
#include "towerengine.h"

void CDefaultFaceShader::Init(void)
{
	printf("Compiling Face Shader...\n");
	SetSource(default_face_shader_vert, default_face_shader_frag);
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

	directional_light_count_uniform = glGetUniformLocationARB(program, "directional_light_count_uni");
	directional_light_dir_uniform = glGetUniformLocationARB(program, "directional_light_dir_uni");
	directional_light_color_uniform = glGetUniformLocationARB(program, "directional_light_color_uni");
	directional_light_shadow_enabled_uniform = glGetUniformLocationARB(program, "directional_light_shadow_enabled_uni");
	directional_light_shadow_clip_uniform = glGetUniformLocationARB(program, "directional_light_shadow_clip_uni");
	directional_light_shadow_tex_matrix_uniform = glGetUniformLocationARB(program, "directional_light_shadow_tex_matrix_uni");
	directional_light_shadow_map_uniform = glGetUniformLocationARB(program, "directional_light_shadow_map_uni");

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

	transformation_uniform = glGetUniformLocationARB(program, "transformation_uni");

	clip_uniform = glGetUniformLocationARB(program, "clip_vec_uni");
	clip_dist_uniform = glGetUniformLocationARB(program, "clib_dist_uni");

	ResetUniforms();
	UseNoShader();
}

void CDefaultFaceShader::SetTransformation(const float m[16])
{
	if(transformation_uniform != -1)
		glUniformMatrix4fvARB(transformation_uniform, 1, GL_FALSE, m);
}

void CDefaultFaceShader::SetDiffuseColor(CVector color)
{
	if(diffuse_color_uniform != -1)
		glUniform3fARB(diffuse_color_uniform, color.x, color.y, color.z);
}

void CDefaultFaceShader::SetDiffuseColor2(CVector color, float alpha)
{
	if(diffuse_color2_uniform != -1)
		glUniform4fARB(diffuse_color2_uniform, color.x, color.y, color.z, alpha);
}

void CDefaultFaceShader::SetSpecularColor(CVector color)
{
	if(specular_color_uniform != -1)
		glUniform3fARB(specular_color_uniform, color.x, color.y, color.z);
}

void CDefaultFaceShader::SetAmbient(float ambient)
{
	if(ambient_uniform != -1)
		glUniform1fARB(ambient_uniform, ambient);
}

void CDefaultFaceShader::SetPointLights(int count, float *pos, float *color, float *dist, int *shadow_enabled, GLuint *shadow_maps)
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
}

void CDefaultFaceShader::SetDirectionalLights(int count, float *dir, float *color, int *shadow_enabled, GLuint *shadow_maps, float *shadow_clip, float *shadow_tex_martix)
{
	int i;

	count = min(count, max_directional_lights);
	glUniform1iARB(directional_light_count_uniform, count);
	glUniform3fvARB(directional_light_dir_uniform, count, dir);
	glUniform3fvARB(directional_light_color_uniform, count, color);
	glUniform1ivARB(directional_light_shadow_enabled_uniform, count, shadow_enabled);
	glUniform2fvARB(directional_light_shadow_clip_uniform, count, shadow_clip);
	glUniformMatrix4fvARB(directional_light_shadow_tex_matrix_uniform, count, GL_FALSE, shadow_tex_martix);

	int *tex_units = new int[max_directional_lights];
	for(i=0; i<count; i++)
	{
		tex_units[i] = directional_light_shadow_tex_first_unit + i;
		glActiveTextureARB(GL_TEXTURE0 + tex_units[i]);
		glBindTexture(GL_TEXTURE_2D, shadow_maps[i]);
	}
	for(i=count; i<max_directional_lights; i++)
		tex_units[i] = directional_light_shadow_tex_first_unit + i;

	glUniform1ivARB(directional_light_shadow_map_uniform, max_directional_lights, tex_units);
}

void CDefaultFaceShader::SetLightAmbientColor(CVector color)
{
	if(light_ambient_color_uniform != -1)
		glUniform3fARB(light_ambient_color_uniform, color.x, color.y, color.z);
}

void CDefaultFaceShader::SetSpecular(float size)
{
	if(specular_size_uniform != -1)
		glUniform1f(specular_size_uniform, size);

}


void CDefaultFaceShader::SetDiffuseTexture(bool enabled, GLuint tex)
{
	glUniform1iARB(diffuse_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1iARB(diffuse_tex_uniform, diffuse_tex_unit);
		glActiveTexture(GL_TEXTURE0 + diffuse_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}

void CDefaultFaceShader::SetSpecularTexture(bool enabled, GLuint tex)
{
	glUniform1iARB(specular_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1iARB(specular_tex_uniform, specular_tex_unit);
		glActiveTexture(GL_TEXTURE0 + specular_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

}

void CDefaultFaceShader::SetNormalTexture(bool enabled, GLuint tex)
{
	glUniform1iARB(normal_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1iARB(normal_tex_uniform, normal_tex_unit);
		glActiveTexture(GL_TEXTURE0 + normal_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

}

void CDefaultFaceShader::SetHeightTexture(bool enabled, GLuint tex)
{
	glUniform1iARB(height_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1iARB(height_tex_uniform, height_tex_unit);
		glActiveTexture(GL_TEXTURE0 + height_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}


void CDefaultFaceShader::SetTexCoord(CVector2 coord)
{
	glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, coord.v);
}

void CDefaultFaceShader::SetBorder(int e, CVector2 a, CVector2 b)
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

void CDefaultFaceShader::SetVectors(CVector normal, CVector tangx, CVector tangy, CVector fnormal)
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

void CDefaultFaceShader::SetHeightFactor(CVector factor)
{
	if(height_factor_uniform != -1)
		glUniform3f(height_factor_uniform, factor.x, factor.y, factor.z);
}

void CDefaultFaceShader::SetBumpFactor(float f)
{
	if(bump_factor_uniform != -1)
		glUniform1f(bump_factor_uniform, f);
}

void CDefaultFaceShader::SetTwoSide(int v)
{
	if(two_side_uniform != -1)
		glUniform1i(two_side_uniform, v ? 1 : 0);
}

void CDefaultFaceShader::SetFog(CVector color, float depth, float dist)
{
	if(fog_color_uniform != -1)
		glUniform3f(fog_color_uniform, color.x, color.y, color.z);
	if(fog_depth_uniform != -1)
		glUniform1f(fog_depth_uniform, depth);
	if(fog_dist_uniform != -1)
		glUniform1f(fog_dist_uniform, dist);
}

void CDefaultFaceShader::SetClip(CVector c, float d)
{
	glUniform3f(clip_uniform, c.x, c.y, c.z);
	glUniform1f(clip_dist_uniform, d);
}

void CDefaultFaceShader::SetVertexMix(float m)
{
	glUniform1f(vertex_mix_uniform, m);
}

void CDefaultFaceShader::ResetUniforms(void)
{
	SetVectors(Vec(0.0, 0.0, 1.0), Vec(1.0, 0.0, 0.0), Vec(0.0, 1.0, 0.0));
	SetDiffuseColor(Vec(1.0, 1.0, 1.0));
	SetSpecularColor(Vec(0.5, 0.5, 0.5));
	SetAmbient(1.0);
	SetPointLights(0, 0, 0, 0, 0, 0);
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
