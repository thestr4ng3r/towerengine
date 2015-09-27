
#include "towerengine.h"
#include "tresources.h"

void tDepthPassShader::Init(void)
{
	InitShader(depth_pass_shader_vert, depth_pass_shader_frag, "Depth Pass Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	//glBindAttribLocation(program, uvcoord_attribute, "uv_attr");

	LinkProgram();

	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
	diffuse_tex_enabled_uniform = GetUniformLocation("diffuse_tex_enabled_uni");

	transformation_uniform = GetUniformLocation("transformation_uni");

	//clip_uniform = GetUniformLocation("clip_vec_uni");
	//clip_dist_uniform = GetUniformLocation("clib_dist_uni");

	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "MatrixBlock"), matrix_binding_point);

	//Bind();
	//glUniform1i(diffuse_tex_uniform, diffuse_tex_unit);
}


void tDepthPassShader::SetTransformation(const float m[16])
{
	glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}

void tDepthPassShader::SetDiffuseTexture(bool enabled, GLuint tex)
{
	glUniform1i(diffuse_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glActiveTexture(GL_TEXTURE0 + diffuse_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}