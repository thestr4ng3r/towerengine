
#include "towerengine.h"

tBulletDebugObject::tBulletDebugObject()
{
	debug_mode = DBG_DrawWireframe;

	glGenVertexArrays(1, &line_vao);
	glBindVertexArray(line_vao);

	glGenBuffers(1, &line_position_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, line_position_vbo);
	glEnableVertexAttribArray(tColorShader::vertex_attribute);
	glVertexAttribPointer(tColorShader::vertex_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &line_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, line_color_vbo);
	glEnableVertexAttribArray(tColorShader::color_attribute);
	glVertexAttribPointer(tColorShader::color_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

tBulletDebugObject::~tBulletDebugObject()
{
}

void tBulletDebugObject::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
	line_position.push_back(tVec(from));
	line_color.push_back(tVec(color));

	line_position.push_back(tVec(to));
	line_color.push_back(tVec(color));
}

void tBulletDebugObject::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
{
	printf("drawContactPoint");
}

void tBulletDebugObject::reportErrorWarning(const char *warningString)
{
	printf("Bullet Warning: %s\n", warningString);
}

void tBulletDebugObject::draw3dText(const btVector3 &location, const char *textString)
{
	printf("Bullet 3D Text: %s\n", textString);
}

void tBulletDebugObject::ForwardPass(tRenderer *renderer)
{
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	line_position.clear();
	line_color.clear();

	btDynamicsWorld *dynamics_world = GetWorld()->GetDynamicsWorld();
	dynamics_world->debugDrawWorld();

	glBindBuffer(GL_ARRAY_BUFFER, line_position_vbo);
	glBufferData(GL_ARRAY_BUFFER, line_position.size() * sizeof(tVector), line_position.data(), GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, line_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, line_color.size() * sizeof(tVector), line_color.data(), GL_STREAM_DRAW);


	renderer->GetColorShader()->Bind();
	renderer->GetColorShader()->SetModelViewProjectionmatrix(renderer->GetCurrentRenderingCamera()->GetModelViewProjectionMatrix().GetData());
	float t[16];
	tTransform::GetIdentity().GetMatrix(t);
	renderer->GetColorShader()->SetTransformation(t);
	glLineWidth(1.0);

	glBindVertexArray(line_vao);
	glDrawArrays(GL_LINES, 0, line_position.size());
}

void tBulletDebugObject::AddedToWorld(tWorld *world)
{
	btDynamicsWorld *dynamics_world = GetWorld()->GetDynamicsWorld();
	dynamics_world->setDebugDrawer(this);
}

void tBulletDebugObject::RemovedFromWorld(tWorld *world)
{
	btDynamicsWorld *dynamics_world = GetWorld()->GetDynamicsWorld();

	if (dynamics_world->getDebugDrawer() == this)
		dynamics_world->setDebugDrawer(0);
}

void tBulletDebugObject::StandardForwardPass(tForwardRenderer *renderer)
{
}