
#include "towerengine.h"


tCoordinateSystemObject::tCoordinateSystemObject(bool depth_test) : tObject()
{
	this->depth_test = depth_test;
	pos = Vec(0.0, 0.0, 0.0);
	size = Vec(1.0, 1.0, 1.0);

	vao = new tVAO();
	vao->Bind();

	vertex_vbo = new tVBO<float>(3, 6);
	float vertex_data[18] = {
			0.0, 0.0, 0.0,
			1.0, 0.0, 0.0,
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, 0.0, 0.0,
			0.0, 0.0, 1.0
	};
	memcpy(vertex_vbo->GetData(), vertex_data, sizeof(float) * 18);
	vertex_vbo->AssignData();
	vertex_vbo->SetAttribute(tShader::vertex_attribute, GL_FLOAT);

	color_vbo = new tVBO<float>(4, 6);
	float color_data[24] = {
			1.0, 0.0, 0.0, 1.0,
			1.0, 0.0, 0.0, 1.0,
			0.0, 1.0, 0.0, 1.0,
			0.0, 1.0, 0.0, 1.0,
			0.0, 0.0, 1.0, 1.0,
			0.0, 0.0, 1.0, 1.0
	};
	memcpy(color_vbo->GetData(), color_data, sizeof(float) * 24);
	color_vbo->AssignData();
	color_vbo->SetAttribute(tColorShader::color_attribute, GL_FLOAT);
}

tCoordinateSystemObject::~tCoordinateSystemObject(void)
{
	delete vao;
	delete vertex_vbo;
	delete color_vbo;
}

void tCoordinateSystemObject::ForwardPass(tRenderer *renderer)
{
	if(depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	renderer->GetColorShader()->Bind();
	renderer->GetColorShader()->SetModelViewProjectionmatrix(renderer->GetCurrentRenderingCamera()->GetModelViewProjectionMatrix().GetData());
	renderer->GetColorShader()->SetTransformation(tMatrix4::identity_matrix);

	glLineWidth(3.0);

	vao->Draw(GL_LINES, 0, 6);
}

tBoundingBox tCoordinateSystemObject::GetBoundingBox(void)
{
	tBoundingBox b;

	b.SetBounds(pos, pos + size);

	return b;
}
