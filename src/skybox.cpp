
#include "towerengine.h"

tSkyBox::tSkyBox(GLuint cube_map, float size)
{
	this->size = size;

	vao = new tVAO();
	vbo = new tVBO<float>(3, vao, 8);

	float vertices[24] = { 	-size, size, -size, // top
							size, size, -size,
							size, size, size,
							-size, size, size,

							-size, -size, -size, // bottom
							size, -size, -size,
							size, -size, size,
							-size, -size, size };

	memcpy(vbo->GetData(), vertices, 24*sizeof(float));
	vbo->AssignData();

	ibo = new tIBO(vao, 24);

	GLuint indices[24] = { 	0, 1, 2, 3, // top
							7, 6, 5, 4, // bottom
							0, 3, 7, 4, // left
							2, 1, 5, 6, // right
							3, 2, 6, 7, // front
							1, 0, 4, 5  // back
						};

	memcpy(ibo->GetData(), indices, 24 * sizeof(GLuint));
	ibo->AssignData();

	SetCubeMap(cube_map);
}

void tSkyBox::Paint(tRenderer *renderer, tVector pos)
{
	renderer->GetSkyBoxShader()->Bind();
	renderer->GetSkyBoxShader()->SetCubeMap(cube_map);

	vbo->SetAttribute(tSkyBoxShader::vertex_attribute, GL_FLOAT);

	ibo->Draw(GL_QUADS);
}
