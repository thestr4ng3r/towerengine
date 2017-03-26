
#include "towerengine.h"
#include "resources.h"

tDepthPassShader::tDepthPassShader(void)
{
	InitShader(resources_get("depth_pass_shader.vert"), resources_get("depth_pass_shader.frag"), "Depth Pass Shader");

	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, uvcoord_attribute, "uv_attr");

	LinkProgram();

	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
	diffuse_tex_enabled_uniform = GetUniformLocation("diffuse_tex_enabled_uni");

	transformation_uniform = GetUniformLocation("transformation_uni");

	//clip_uniform = GetUniformLocation("clip_vec_uni");
	//clip_dist_uniform = GetUniformLocation("clib_dist_uni");

	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "MatrixBlock"), matrix_binding_point);

	Bind();
	glUniform1i(diffuse_tex_uniform, base_color_tex_unit);
}


void tDepthPassShader::SetTransformation(const float m[16])
{
	glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, m);
}

void tDepthPassShader::SetBaseColorTexture(bool enabled, GLuint tex)
{
	glUniform1i(diffuse_tex_enabled_uniform, enabled ? 1 : 0);

	if(enabled)
	{
		glActiveTexture(GL_TEXTURE0 + base_color_tex_unit);
		glBindTexture(GL_TEXTURE_2D, tex);
	}
}
