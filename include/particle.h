
#ifndef _PARTICLE_H
#define _PARTICLE_H


struct tParticle
{
	tVector position;
	tVector velocity;
};


class tParticleSystem
{
	private:
		std::list<tParticle *> particles;

		tVAO *vao;
		GLuint vbo;

		std::vector<float> vertex_data;

		tVector center;
		float radius;
		float velocity_min;
		float velocity_max;
		float spawn_time;

		float last_spawn_time;

	public:
		tParticleSystem(tVector center, float radius, float velocity_min, float velocity_max, float spawn_time);
		~tParticleSystem(void);

		void Step(float time);

		void Render(tRenderer *renderer);
};



#endif
