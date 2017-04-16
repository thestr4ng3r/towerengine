
#ifndef _TOWERENGINE_BULLET_DEBUG_OBJECT_H
#define _TOWERENGINE_BULLET_DEBUG_OBJECT_H

class tBulletDebugObject : public tObject, public btIDebugDraw
{
	private:
		int debug_mode;

		std::vector<tVector> line_position;
		std::vector<tVector> line_color;

		GLuint line_position_vbo;
		GLuint line_color_vbo;
		GLuint line_vao;

	public:
		tBulletDebugObject();
		virtual ~tBulletDebugObject();

		void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color);
		void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color);
		void reportErrorWarning(const char *warningString);
		void draw3dText(const btVector3 &location, const char *textString);

		void setDebugMode(int debug_mode)	{ this->debug_mode = debug_mode; }
		int getDebugMode() const			{ return debug_mode; }

		void AddedToWorld(tWorld *world);
		void RemovedFromWorld(tWorld *world);

		void ForwardPass(tRenderer *);
		void StandardForwardPass(tForwardRenderer *);
};

#endif