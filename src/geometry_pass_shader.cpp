
#include "towerengine.h"

void CGeometryPassShader::Init(void)
{
	SetSource(geometry_pass_shader_vert, geometry_pass_shader_frag);
	CreateVertexShader();
	CreateFragmentShader();
	CreateProgram();

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, vertex2_attribute, "vertex2_attr");
	glBindAttribLocation(program, normal_attribute, "normal_attr");
	glBindAttribLocation(program, tang_attribute, "tang_attr");
	glBindAttribLocation(program, bitang_attribute, "bitang_attr");
	glBindAttribLocation(program, uvcoord_attribute, "uv_attr");

	LinkProgram();

	vertex_mix_uniform = GetUniformLocation("vertex_mix_uni");

	diffuse_color_uniform = GetUniformLocation("diffuse_color_uni");
	diffuse_color2_uniform = GetUniformLocation("diffuse_color2_uni");
	specular_color_uniform = GetUniformLocation("specular_color_uni");

	specular_size_uniform = GetUniformLocation("specular_size_uni");

	bump_depth_uniform = GetUniformLocation("bump_depth_uni");

	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	specular_tex_uniform = GetUniformLocation("specular_tex_uni");
	bump_tex_uniform = GetUniformLocation("bump_tex_uni");

	diffuse_tex_enabled_uniform = GetUniformLocation("diffuse_tex_enabled_uni");
	normal_tex_enabled_uniform = GetUniformLocation("normal_tex_enabled_uni");
	specular_tex_enabled_uniform = GetUniformLocation("specular_tex_enabled_uni");
	bump_tex_enabled_uniform = GetUniformLocation("bump_tex_enabled_uni");

	transformation_uniform = GetUniformLocation("transformation_uni");

	clip_uniform = GetUniformLocation("clip_vec_uni");
	clip_dist_uniform = GetUniformLocation("clib_dist_uni");

	Bind();
	ResetUniforms();
	Unbind();
}

void CGeometryPassShader::SetTransformation(const float m[16])
{
	if(transformation_uniform != -1)
		glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}

void CGeometryPassShader::SetDiffuseColor(CVector color)
{
	if(diffuse_color_uniform != -1)
		glUniform3f(diffuse_color_uniform, color.x, color.y, color.z);
}

void CGeometryPassShader::SetDiffuseColor2(CVector color, float alpha)
{
	if(diffuse_color2_uniform != -1)
		glUniform4f(diffuse_color2_uniform, color.x, color.y, color.z, alpha);
}

void CGeometryPassShader::SetSpecularColor(CVector color)
{
	if(specular_color_uniform != -1)
		glUniform3f(specular_color_uniform, color.x, color.y, color.z);
}


void CGeometryPassShader::SetSpecular(float size)
{
	if(specular_size_uniform != -1)
		glUniform1f(specular_size_uniform, size);

}


void CGeometryPassShader::SetDiffuseTexture(bool enabled, GLuint tex)
{
	glUniform1i(diffuse_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1i(diffuse_tex_uniform, diffuse_tex_unit);
		glActiveTexture(GL_TEXTURE0 + diffuse_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}

void CGeometryPassShader::SetSpecularTexture(bool enabled, GLuint tex)
{
	glUniform1i(specular_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1i(specular_tex_uniform, specular_tex_unit);
		glActiveTexture(GL_TEXTURE0 + specular_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

}

void CGeometryPassShader::SetNormalTexture(bool enabled, GLuint tex)
{
	glUniform1i(normal_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1i(normal_tex_uniform, normal_tex_unit);
		glActiveTexture(GL_TEXTURE0 + normal_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

}

void CGeometryPassShader::SetBumpDepth(float depth)
{
	glUniform1f(bump_depth_uniform, depth);
}

void CGeometryPassShader::SetBumpTexture(bool enabled, GLuint tex)
{
	glUniform1i(bump_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1i(bump_tex_uniform, bump_tex_unit);
		glActiveTexture(GL_TEXTURE0 + bump_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}


void CGeometryPassShader::SetTexCoord(CVector2 coord)
{
	glMultiTexCoord2fv(GL_TEXTURE0, coord.v);
}


void CGeometryPassShader::SetVectors(CVector normal, CVector tangx, CVector tangy, CVector fnormal)
{
	if(normal.SquaredLen() < 0.5)
		printf("%f, %f, %f\n", pvec(normal));
	normal.AttrToGL(normal_attribute);

	if(fnormal.SquaredLen() > 0.0)
		fnormal.AttrToGL(normal_attribute);
	else
		normal.AttrToGL(normal_attribute);
}



void CGeometryPassShader::SetClip(CVector c, float d)
{
	glUniform3f(clip_uniform, c.x, c.y, c.z);
	glUniform1f(clip_dist_uniform, d);
}

void CGeometryPassShader::SetVertexMix(float m)
{
	glUniform1f(vertex_mix_uniform, m);
}

void CGeometryPassShader::ResetUniforms(void)
{
	SetVectors(Vec(0.0, 0.0, 1.0), Vec(1.0, 0.0, 0.0), Vec(0.0, 1.0, 0.0));
	SetDiffuseColor(Vec(1.0, 1.0, 1.0));
	SetSpecularColor(Vec(0.5, 0.5, 0.5));
	SetPointLights(0, 0, 0, 0, 0, 0);
	SetLightAmbientColor(Vec(0.0, 0.0, 0.0));
	SetSpecular(32.0);
	SetClip(Vec(0.0, 0.0, 0.0), 0.0);
	SetVertexMix(0.0);
	SetDiffuseTexture(false);
	SetSpecularTexture(false);
	SetNormalTexture(false);
}
