#include "towerengine.h"
#include "resources.h"

using namespace std;


tSkyBoxShader::tSkyBoxShader(void)
{
	InitShader(resources_get("skybox_shader.vert"), resources_get("skybox_shader.frag"), "SkyBox Shader");
	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	LinkProgram();

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	tex_uniform = GetUniformLocation("cube_map_uni");


	glUniformBlockBinding(program, glGetUniformBlockIndex(program, "MatrixBlock"), matrix_binding_point);

	Bind();
	glUniform1i(tex_uniform, 0);
}

void tSkyBoxShader::SetCubeMap(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
}

void tSkyBoxShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}


GLenum CubeTex(int side)
{
	switch(side)
	{
		case 0:	return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		case 1:	return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		case 2:	return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		case 3:	return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		case 4:	return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		case 5:	return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
	}

	return 0;
}

int CubeSide(GLenum tex)
{
	switch(tex)
	{
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:	return 0;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:	return 1;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:	return 2;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:	return 3;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:	return 4;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:	return 5;
	}

	return 0;
}

tVector CubeVecS(int side)
{
	switch(side)
	{
		case 0:	return tVec(1.0, 0.0, 0.0);
		case 1:	return tVec(-1.0, 0.0, 0.0);
		case 2:	return tVec(0.0, 1.0, 0.0);
		case 3:	return tVec(0.0, -1.0, 0.0);
		case 4:	return tVec(0.0, 0.0, 1.0);
		case 5:	return tVec(0.0, 0.0, -1.0);
	}

	return tVec(0.0, 0.0, 0.0);
}

