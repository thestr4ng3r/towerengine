
#include "towerengine.h"


CCoordinateSystemObject::CCoordinateSystemObject(bool depth_test) : CObject()
{
	this->depth_test = depth_test;
	pos = Vec(0.0, 0.0, 0.0);
	size = Vec(1.0, 1.0, 1.0);
}

CCoordinateSystemObject::~CCoordinateSystemObject(void)
{

}

void CCoordinateSystemObject::ForwardPass(void)
{
	if(depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	CEngine::GetColorShader()->Bind();
	CEngine::GetColorShader()->SetTransformation(CEngine::identity_matrix4);

	glLineWidth(3.0);

	CEngine::GetColorShader()->SetDiffuseColor(Vec(1.0, 0.0, 0.0));
	glBegin(GL_LINES);
	pos.AttrToGL(CFaceShader::vertex_attribute);
	(pos + size * Vec(1.0, 0.0, 0.0)).AttrToGL(CFaceShader::vertex_attribute);
	glEnd();

	CEngine::GetColorShader()->SetDiffuseColor(Vec(0.0, 1.0, 0.0));
	glBegin(GL_LINES);
	glColor4f(0.0, 1.0, 0.0, 1.0);
	pos.AttrToGL(CFaceShader::vertex_attribute);
	(pos + size * Vec(0.0, 1.0, 0.0)).AttrToGL(CFaceShader::vertex_attribute);
	glEnd();

	CEngine::GetColorShader()->SetDiffuseColor(Vec(0.0, 0.0, 1.0));
	glBegin(GL_LINES);
	glColor4f(0.0, 0.0, 1.0, 1.0);
	pos.AttrToGL(CFaceShader::vertex_attribute);
	(pos + size * Vec(0.0, 0.0, 1.0)).AttrToGL(CFaceShader::vertex_attribute);
	glEnd();
}

CBoundingBox CCoordinateSystemObject::GetBoundingBox(void)
{
	CBoundingBox b;

	b.SetBounds(pos, pos + size);

	return b;
}
