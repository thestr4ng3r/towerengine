
#include "towerengine.h"

void CSkyBox::Paint(CVector pos, float size)
{
	glDisable(GL_DEPTH_TEST);

	CEngine::GetSkyBoxShader()->BindShader();
	CEngine::GetSkyBoxShader()->SetCubeMap(cube_map);

	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);

	glBegin(GL_QUADS);
		glVertex3f(size, size, size);
		glVertex3f(size, -size, size);
		glVertex3f(-size, -size, size);
		glVertex3f(-size, size, size);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(size, size, -size);
		glVertex3f(size, size, size);
		glVertex3f(-size, size, size);
		glVertex3f(-size, size, -size);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(-size, size, -size);
		glVertex3f(-size, -size, -size);
		glVertex3f(size, -size, -size);
		glVertex3f(size, size, -size);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(size, -size, -size);
		glVertex3f(-size, -size, -size);
		glVertex3f(-size, -size, size);
		glVertex3f(size, -size, size);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(-size, size, size);
		glVertex3f(-size, -size, size);
		glVertex3f(-size, -size, -size);
		glVertex3f(-size, size, -size);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(size, size, -size);
		glVertex3f(size, -size, -size);
		glVertex3f(size, -size, size);
		glVertex3f(size, size, size);
	glEnd();

	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
}
