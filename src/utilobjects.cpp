
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
	CShader::Unbind();
	if(depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	glLineWidth(3.0);
	glBegin(GL_LINES);

	glColor4f(1.0, 0.0, 0.0, 1.0);
	pos.PutToGL();
	(pos + size * Vec(1.0, 0.0, 0.0)).PutToGL();

	glColor4f(0.0, 1.0, 0.0, 1.0);
	pos.PutToGL();
	(pos + size * Vec(0.0, 1.0, 0.0)).PutToGL();

	glColor4f(0.0, 0.0, 1.0, 1.0);
	pos.PutToGL();
	(pos + size * Vec(0.0, 0.0, 1.0)).PutToGL();

	glEnd();
}

CBoundingBox CCoordinateSystemObject::GetBoundingBox(void)
{
	CBoundingBox b;

	b.SetBounds(pos, pos + size);

	return b;
}
