
#include "towerengine.h"


tCoordinateSystemObject::tCoordinateSystemObject(bool depth_test) : tObject()
{
	this->depth_test = depth_test;
	pos = Vec(0.0, 0.0, 0.0);
	size = Vec(1.0, 1.0, 1.0);
}

tCoordinateSystemObject::~tCoordinateSystemObject(void)
{

}

void tCoordinateSystemObject::ForwardPass(tRenderer *renderer)
{
	if(depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	renderer->GetColorShader()->Bind();
	renderer->GetColorShader()->SetTransformation(tMatrix4::identity_matrix);

	glLineWidth(3.0);

	renderer->GetColorShader()->SetDiffuseColor(Vec(1.0, 0.0, 0.0));
	glBegin(GL_LINES);
	pos.AttrToGL(tFaceShader::vertex_attribute);
	(pos + size * Vec(1.0, 0.0, 0.0)).AttrToGL(tFaceShader::vertex_attribute);
	glEnd();

	renderer->GetColorShader()->SetDiffuseColor(Vec(0.0, 1.0, 0.0));
	glBegin(GL_LINES);
	glColor4f(0.0, 1.0, 0.0, 1.0);
	pos.AttrToGL(tFaceShader::vertex_attribute);
	(pos + size * Vec(0.0, 1.0, 0.0)).AttrToGL(tFaceShader::vertex_attribute);
	glEnd();

	renderer->GetColorShader()->SetDiffuseColor(Vec(0.0, 0.0, 1.0));
	glBegin(GL_LINES);
	glColor4f(0.0, 0.0, 1.0, 1.0);
	pos.AttrToGL(tFaceShader::vertex_attribute);
	(pos + size * Vec(0.0, 0.0, 1.0)).AttrToGL(tFaceShader::vertex_attribute);
	glEnd();
}

tBoundingBox tCoordinateSystemObject::GetBoundingBox(void)
{
	tBoundingBox b;

	b.SetBounds(pos, pos + size);

	return b;
}
