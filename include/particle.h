
#ifndef _PARTICLE_H
#define _PARTICLE_H


struct tParticle
{
	struct vertex_data
	{
		tVector position;
		float size;
		float rotation;
		tVector color;
		float alpha;
	} vertex_data;

	float texture_index;

	tVector velocity;
	float rotation_velocity;

	float lifetime;
	float lifetime_death;

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
		GLuint vertex_data_vbo;
		GLuint texture_index_vbo;

		std::vector<float> vertex_data;
		std::vector<float> texture_index_data;

		SpawnAreaType spawn_area_type;
		tVector center;
		tVector spawn_size;
		float spawn_time;

		float size_min;
		float size_max;

		float velocity_min;
		float velocity_max;

		float rotation_velocity_min;
		float rotation_velocity_max;

		float fade_in_time;
		float fade_out_time;

		float lifetime_min;
		float lifetime_max;

		tVector color;

		bool gravity_enabled;
		tVector gravity;

		BlendType blend_type;

		float max_dist;

		float last_spawn_time;

		GLuint tex;
		int tex_count;

	public:
		tParticleSystem(tVector center, GLuint tex, int tex_count);
		~tParticleSystem(void);

		void Step(float time);

		void Render(tRenderer *renderer);

		void SetGravity(tVector gravity)							{ this->gravity = gravity; this->gravity_enabled = gravity.SquaredLen() > 0.0; }
		void SetMaxParticleDistance(float dist)						{ this->max_dist = dist; }
		void SetBlendType(BlendType type)							{ this->blend_type = type; }
		void SetSpawnArea(SpawnAreaType area_type, tVector size)	{ this->spawn_area_type = area_type; this->spawn_size = size; }
		void SetVelocity(float min, float max)						{ this->velocity_min = min; this->velocity_max = max; }
		void SetRotationVelocity(float min, float max)				{ this->rotation_velocity_min = min; this->rotation_velocity_max = max; }
		void SetSpawnInterval(float time)							{ this->spawn_time = time; }
		void SetSize(float min, float max)							{ this->size_min = min; this->size_max = max; }
		void SetFadeIn(float time)									{ this->fade_in_time = time; }
		void SetFadeOut(float time)									{ this->fade_out_time = time; }
		void SetLifetime(float min, float max)						{ this->lifetime_min = min; this->lifetime_max = max; }
		void SetColor(tVector color)								{ this->color = color; }

		int GetParticlesCount(void)									{ return particles.size(); }
};



#endif
