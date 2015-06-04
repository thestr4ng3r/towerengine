
#include "towerengine.h"

tSkyBox::tSkyBox(GLuint cube_map, float size)
{
	this->size = size;

	vao = new tVAO();
	vao->Bind();

	vbo = new tVBO<float>(3, 8);

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
	vbo->SetAttribute(tSkyBoxShader::vertex_attribute, GL_FLOAT);

	ibo = new tIBO(54);

	GLuint indices[54] = { 	0, 1, 2,	0, 2, 3, // top
							7, 6, 5,	7, 5, 4, // bottom
							0, 3, 7,	0, 7, 4, // left
							2, 1, 5,	2, 5, 6, // right
							3, 2, 6,	3, 6, 7, // front
							1, 0, 4,	1, 4, 5  // back
						};

	memcpy(ibo->GetData(), indices, 54 * sizeof(GLuint));
	ibo->AssignData();

	SetCubeMap(cube_map);
}

void tSkyBox::Paint(tRenderer *renderer, tVector pos)
{
	renderer->GetSkyBoxShader()->SetCubeMap(cube_map);
	vao->Bind();
	ibo->Draw(GL_TRIANGLES);
}
