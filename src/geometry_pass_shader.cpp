
#include "towerengine.h"

void CGeometryPassShader::Init(void)
{
	SetSource(geometry_pass_shader_vert, geometry_pass_shader_frag);
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

	vertex_mix_uniform = GetUniformLocation("vertex_mix_uni");

	diffuse_color_uniform = GetUniformLocation("diffuse_color_uni");
	diffuse_color2_uniform = GetUniformLocation("diffuse_color2_uni");
	specular_color_uniform = GetUniformLocation("specular_color_uni");



	specular_size_uniform = GetUniformLocation("specular_size_uni");

	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	//height_tex_uniform = GetUniformLocation("height_tex_uni");
	specular_tex_uniform = GetUniformLocation("specular_tex_uni");

	diffuse_tex_enabled_uniform = GetUniformLocation("diffuse_tex_enabled_uni");
	normal_tex_enabled_uniform = GetUniformLocation("normal_tex_enabled_uni");
	//height_tex_enabled_uniform = GetUniformLocation("height_tex_enabled_uni");
	specular_tex_enabled_uniform = GetUniformLocation("specular_tex_enabled_uni");

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
		glUniformMatrix4fvARB(transformation_uniform, 1, GL_FALSE, m);
}

void CGeometryPassShader::SetDiffuseColor(CVector color)
{
	if(diffuse_color_uniform != -1)
		glUniform3fARB(diffuse_color_uniform, color.x, color.y, color.z);
}

void CGeometryPassShader::SetDiffuseColor2(CVector color, float alpha)
{
	if(diffuse_color2_uniform != -1)
		glUniform4fARB(diffuse_color2_uniform, color.x, color.y, color.z, alpha);
}

void CGeometryPassShader::SetSpecularColor(CVector color)
{
	if(specular_color_uniform != -1)
		glUniform3fARB(specular_color_uniform, color.x, color.y, color.z);
}


void CGeometryPassShader::SetSpecular(float size)
{
	if(specular_size_uniform != -1)
		glUniform1f(specular_size_uniform, size);

}


void CGeometryPassShader::SetDiffuseTexture(bool enabled, GLuint tex)
{
	glUniform1iARB(diffuse_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1iARB(diffuse_tex_uniform, diffuse_tex_unit);
		glActiveTexture(GL_TEXTURE0 + diffuse_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}

void CGeometryPassShader::SetSpecularTexture(bool enabled, GLuint tex)
{
	glUniform1iARB(specular_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1iARB(specular_tex_uniform, specular_tex_unit);
		glActiveTexture(GL_TEXTURE0 + specular_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

}

void CGeometryPassShader::SetNormalTexture(bool enabled, GLuint tex)
{
	glUniform1iARB(normal_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1iARB(normal_tex_uniform, normal_tex_unit);
		glActiveTexture(GL_TEXTURE0 + normal_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

}

/*void CDefaultFaceShader::SetHeightTexture(bool enabled, GLuint tex)
{
	glUniform1iARB(height_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glUniform1iARB(height_tex_uniform, height_tex_unit);
		glActiveTexture(GL_TEXTURE0 + height_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}*/


void CGeometryPassShader::SetTexCoord(CVector2 coord)
{
	glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, coord.v);
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
