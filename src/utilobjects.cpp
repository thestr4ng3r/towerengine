
#include "towerengine.h"

tLineObject::tLineObject(tVector position_a, tVector position_b)
{
	this->positions[0] = position_a;
	this->positions[1] = position_b;
	this->colors[0] = tVec(1.0f, 1.0f, 1.0f);
	this->colors[1] = tVec(1.0f, 1.0f, 1.0f);
	line_width = 1.0f;

	vao = new tVAO();
	vao->Bind();

	vertex_vbo = new tVBO<float>(3, 2);
	vertex_vbo->SetAttribute(tShader::vertex_attribute, GL_FLOAT);

	color_vbo = new tVBO<float>(4, 2);
	color_vbo->SetAttribute(tColorShader::color_attribute, GL_FLOAT);

	UpdateVertexVBO();
	UpdateColorVBO();
}

tLineObject::~tLineObject()
{
	delete vao;
	delete vertex_vbo;
	delete color_vbo;
}

void tLineObject::SetPositions(tVector position_a, tVector position_b)
{
	this->positions[0] = position_a;
	this->positions[1] = position_b;
	UpdateVertexVBO();
}

void tLineObject::SetColor(tVector color)
{
	this->colors[0] = color;
	this->colors[1] = color;
	UpdateColorVBO();
}

void tLineObject::SetColors(tVector color_a, tVector color_b)
{
	this->colors[0] = color_a;
	this->colors[1] = color_b;
	UpdateColorVBO();
}

void tLineObject::ForwardPass(tRenderer *renderer)
{
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	renderer->GetColorShader()->Bind();
	renderer->GetColorShader()->SetModelViewProjectionmatrix(renderer->GetCurrentRenderingCamera()->GetModelViewProjectionMatrix().GetData());
	renderer->GetColorShader()->SetTransformation(tMatrix4::identity_matrix);

	glLineWidth(line_width);

	vao->Draw(GL_LINES, 0, 2);
}

tAABB tLineObject::GetAABB(void)
{
	return tAABB(positions[0], positions[1]);
}

void tLineObject::UpdateVertexVBO()
{
	memcpy(vertex_vbo->GetData(), positions, sizeof(float) * 6);
	vertex_vbo->AssignData();
}

void tLineObject::UpdateColorVBO()
{
	float *data = color_vbo->GetData();
	memcpy(data, colors, sizeof(float) * 3);
	data[3] = 1.0f;
	memcpy(data + 4, colors + 1, sizeof(float) * 3);
	data[7] = 1.0f;
	color_vbo->AssignData();
}



// ------------------------------------------------------------

tCoordinateSystemObject::tCoordinateSystemObject(bool depth_test) : tObject()
{
	this->depth_test = depth_test;
	pos = tVec(0.0, 0.0, 0.0);
	size = tVec(1.0, 1.0, 1.0);

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

	float m[16] = { size.x,	0.0f,	0.0f,	pos.x,
					0.0f,	size.y,	0.0f,	pos.y,
					0.0f,	0.0f,	size.z, pos.z,
					0.0f,	0.0f,	0.0f,	1.0f };
	renderer->GetColorShader()->SetTransformation(m);

	glLineWidth(3.0);

	vao->Draw(GL_LINES, 0, 6);
}

tAABB tCoordinateSystemObject::GetAABB(void)
{
	return tAABB(pos, pos + size);
}
