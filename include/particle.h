
#ifndef _PARTICLE_H
#define _PARTICLE_H


struct tParticle
{
	struct
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
		enum RenderType { DEFERRED_LIT, FORWARD_ADD, FORWARD_ALPHA };

	protected:
		std::vector<tParticle> particles;

		tParticleSystem(tVector position, GLuint tex, int tex_count);

	private:
		tVector position;

		tVAO *vao;
		GLuint vertex_data_vbo;
		GLuint texture_index_vbo;

		std::vector<float> vertex_data;
		std::vector<float> texture_index_data;

		RenderType render_type;
		tVector lighting_normal;

		GLuint tex;
		int tex_count;

	public:
		virtual ~tParticleSystem(void);

		virtual void Step(float time) 				{}

		void Render(tRenderer *renderer, tParticleShader *shader);

		void SetPosition(tVector pos)				{ this->position = pos; }
		void SetRenderType(RenderType type)			{ this->render_type = type; }
		void SetLightingNormal(tVector normal)		{ this->lighting_normal = normal; this->lighting_normal.Normalize(); }

		tVector GetPosition(void)					{ return position; }
		RenderType GetRenderType(void)				{ return render_type; }
		int GetTexturesCount(void)					{ return tex_count; }
		unsigned int GetParticlesCount(void)		{ return particles.size(); }
};



class tDefaultParticleSystem : public tParticleSystem
{
	public:
		enum SpawnAreaType { CUBOID, ELLIPSOID };

	private:
		SpawnAreaType spawn_area_type;

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

		float last_spawn_time;

		bool gravity_enabled;
		tVector gravity;

		float max_dist;

	public:
		tDefaultParticleSystem(tVector position, GLuint tex, int tex_count);
		~tDefaultParticleSystem(void);

		virtual void Step(float time);

		void SetSpawnArea(SpawnAreaType area_type, tVector size)	{ this->spawn_area_type = area_type; this->spawn_size = size; }
		void SetVelocity(float min, float max)						{ this->velocity_min = min; this->velocity_max = max; }
		void SetRotationVelocity(float min, float max)				{ this->rotation_velocity_min = min; this->rotation_velocity_max = max; }
		void SetSpawnInterval(float time)							{ this->spawn_time = time; }
		void SetSize(float min, float max)							{ this->size_min = min; this->size_max = max; }
		void SetFadeIn(float time)									{ this->fade_in_time = time; }
		void SetFadeOut(float time)									{ this->fade_out_time = time; }
		void SetLifetime(float min, float max)						{ this->lifetime_min = min; this->lifetime_max = max; }
		void SetColor(tVector color)								{ this->color = color; }
		void SetGravity(tVector gravity)							{ this->gravity = gravity; this->gravity_enabled = gravity.SquaredLen() > 0.0; }
		void SetMaxParticleDistance(float dist)						{ this->max_dist = dist; }
};



class tStaticParticleSystem : public tParticleSystem
{
	public:
		tStaticParticleSystem(tVector position, GLuint tex, int tex_count);
		~tStaticParticleSystem(void);

		void AddParticle(tVector position, float size, float rotation, tVector color, float alpha, int tex);
};



#endif
