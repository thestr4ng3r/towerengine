
#include "towerengine.h"


using namespace std;

tParticleSystem::tParticleSystem(tVector center, GLuint tex, int tex_count)
{
	this->center = center;
	this->tex = tex;
	this->tex_count = tex_count;

	spawn_area_type = ELLIPSOID;
	spawn_size = Vec(1.0, 1.0, 1.0);
	size_min = 0.1;
	size_max = 0.2;
	velocity_min = 0.1;
	velocity_max = 0.2;
	rotation_velocity_min = 0.0;
	rotation_velocity_max = 0.0;
	spawn_time = 0.1;
	max_dist = 100.0;
	blend_type = ALPHA;

	lifetime_min = 2.0;
	lifetime_max = 3.0;

	fade_in_time = 0.0;
	fade_out_time = 0.0;

	color = Vec(1.0, 1.0, 1.0);

	gravity = Vec(0.0, 0.0, 0.0);
	gravity_enabled = false;

	last_spawn_time = 0.0;

	vao = new tVAO();
	vao->Bind();

	glGenBuffers(1, &vertex_data_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_data_vbo);

	glEnableVertexAttribArray(tShader::vertex_attribute);
	glVertexAttribPointer(tShader::vertex_attribute, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);

	glEnableVertexAttribArray(tParticleShader::size_attribute);
	glVertexAttribPointer(tParticleShader::size_attribute, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));

	glEnableVertexAttribArray(tParticleShader::rotation_attribute);
	glVertexAttribPointer(tParticleShader::rotation_attribute, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(4 * sizeof(float)));

	glEnableVertexAttribArray(tParticleShader::color_attribute);
	glVertexAttribPointer(tParticleShader::color_attribute, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(5 * sizeof(float)));

	glGenBuffers(1, &texture_index_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, texture_index_vbo);

	glEnableVertexAttribArray(tParticleShader::texture_index_attribute);
	glVertexAttribPointer(tParticleShader::texture_index_attribute, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	tVAO::UnBind();
}

tParticleSystem::~tParticleSystem(void)
{
	delete vao;
	glDeleteBuffers(1, &vertex_data_vbo);
	glDeleteBuffers(1, &texture_index_vbo);
}

void tParticleSystem::Step(float time)
{
	last_spawn_time += time;

	if(last_spawn_time >= spawn_time)
	{
		int spawn_count = (int)(last_spawn_time / spawn_time);
		last_spawn_time -= (float)spawn_count * spawn_time;

		tVector dir;

		for(int i=0; i<spawn_count; i++)
		{
			dir = Vec(RandomFloat(-1.0, 1.0), RandomFloat(-1.0, 1.0), RandomFloat(-1.0, 1.0));

			if(spawn_area_type == ELLIPSOID)
			{
				while(dir.SquaredLen() > 1.0)
				{
					dir = Vec(RandomFloat(-1.0, 1.0), RandomFloat(-1.0, 1.0), RandomFloat(-1.0, 1.0));
				}
				dir.Normalize();
			}

			tParticle particle;
			particle.vertex_data.position = center + dir * spawn_size * RandomFloat(0.0, 1.0);
			particle.vertex_data.size = RandomFloat(size_min, size_max);
			particle.texture_index = (float)RandomInt(0, tex_count);

			dir.Normalize();
			particle.velocity = RandomFloat(velocity_min, velocity_max) * dir;

			particle.vertex_data.rotation = RandomFloat(0.0, M_PI * 2.0);

			particle.rotation_velocity = RandomFloat(rotation_velocity_min, rotation_velocity_max);

			particle.vertex_data.color = color;
			particle.vertex_data.alpha = 1.0;

			particle.lifetime = 0.0;
			particle.lifetime_death = RandomFloat(lifetime_min, lifetime_max);

			particles.push_back(particle);
		}
	}


	int particles_count = particles.size();
	for(int i=0; i<particles_count; i++)
	{
		tParticle &particle = particles[i];

		particle.lifetime += time;

		if(gravity_enabled)
			particle.velocity += gravity * time;

		particle.vertex_data.position += time * particle.velocity;

		particle.vertex_data.rotation += time * particle.rotation_velocity;

		if(particle.lifetime < fade_in_time)
			particle.vertex_data.alpha = particle.lifetime / fade_in_time;
		else if(particle.lifetime > particle.lifetime_death - fade_out_time)
			particle.vertex_data.alpha = (particle.lifetime_death - particle.lifetime) / fade_out_time;
		else
			particle.vertex_data.alpha = 1.0;


		tVector dir = particle.vertex_data.position - center;
		if(abs(dir.x) > max_dist || abs(dir.y) > max_dist || abs(dir.z) > max_dist
				|| particle.lifetime >= particle.lifetime_death)
		{
			particles_count--;
			particle = particles[particles_count];
			i--;
		}
	}
	particles.resize(particles_count);
}

struct ParticleCompare
{
	bool operator()(const tParticle &a, const tParticle &b)
	{
		return a.compare > b.compare;
	}
};

void tParticleSystem::Render(tRenderer *renderer)
{
	int particles_count = particles.size();

	if(particles_count == 0)
		return;

	if(blend_type == ALPHA)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_TRUE);

		tVector cam_pos = renderer->GetCurrentRenderingCamera()->GetPosition();
		tVector cam_dir = renderer->GetCurrentRenderingCamera()->GetDirection();

		for(vector<tParticle>::iterator pi=particles.begin(); pi!=particles.end(); pi++)
		{
			tParticle &particle = *pi;
			particle.compare = Dot(particle.vertex_data.position - cam_pos, cam_dir);
		}

		sort(particles.begin(), particles.end(), ParticleCompare());
	}
	else // if(blend_type == ADD)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);
	}

	vertex_data.resize(particles_count * 9);
	texture_index_data.resize(particles_count);

	float *vertex_data_v = vertex_data.data();
	float *texture_index_data_v = texture_index_data.data();
	int i=0;
	for(vector<tParticle>::iterator pi=particles.begin(); pi!=particles.end(); pi++)
	{
		tParticle &particle = *pi;
		memcpy(vertex_data_v + i * 9, &particle.vertex_data, sizeof(float) * 9);
		texture_index_data_v[i] = particle.texture_index;
		i++;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertex_data_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data_v, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, texture_index_vbo);
	glBufferData(GL_ARRAY_BUFFER, texture_index_data.size() * sizeof(float), texture_index_data_v, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	renderer->GetParticleShader()->SetTexture(tex);

	vao->Draw(GL_POINTS, 0, particles_count);

	//tVAO::UnBind();
}

