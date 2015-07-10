
#ifndef _PARTICLE_H
#define _PARTICLE_H


struct tParticle
{
	struct vertex_data
	{
		tVector position;
		float size;
	} vertex_data;

	tVector velocity;
	float compare;
};


class tParticleSystem
{
	public:
		enum BlendType { ADD, ALPHA };
		enum SpawnAreaType { CUBOID, ELLIPSOID };

	private:
		std::vector<tParticle> particles;

		tVAO *vao;
		GLuint vbo;

		std::vector<float> vertex_data;

		SpawnAreaType spawn_area_type;
		tVector center;
		tVector spawn_size;
		float spawn_time;

		float velocity_min;
		float velocity_max;

		bool gravity_enabled;
		tVector gravity;

		BlendType blend_type;

		float max_dist;

		float last_spawn_time;

		GLuint tex;

	public:
		tParticleSystem(tVector center, GLuint tex);
		~tParticleSystem(void);

		void Step(float time);

		void Render(tRenderer *renderer);

		void SetGravity(tVector gravity)							{ this->gravity = gravity; this->gravity_enabled = gravity.SquaredLen() > 0.0; }
		void SetMaxParticleDistance(float dist)						{ this->max_dist = dist; }
		void SetBlendType(BlendType type)							{ this->blend_type = type; }
		void SetSpawnArea(SpawnAreaType area_type, tVector size)	{ this->spawn_area_type = area_type; this->spawn_size = size; }
		void SetVelocity(float min, float max)						{ this->velocity_min = min; this->velocity_max = max; }
		void SetSpawnInterval(float time)							{ this->spawn_time = time; }
};



#endif
