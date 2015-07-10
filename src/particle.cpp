
#include "towerengine.h"


using namespace std;

tParticleSystem::tParticleSystem(tVector center, GLuint tex)
{
	this->center = center;
	this->tex = tex;

	spawn_area_type = ELLIPSOID;
	spawn_size = Vec(1.0, 1.0, 1.0);
	velocity_min = 0.1;
	velocity_max = 0.2;
	spawn_time = 0.1;
	max_dist = 100.0;
	blend_type = ALPHA;

	gravity = Vec(0.0, 0.0, 0.0);
	gravity_enabled = false;

	last_spawn_time = 0.0;

	vao = new tVAO();
	vao->Bind();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(tShader::vertex_attribute);
	glVertexAttribPointer(tShader::vertex_attribute, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(tParticleShader::size_attribute);
	glVertexAttribPointer(tParticleShader::size_attribute, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(3 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	tVAO::UnBind();
}

tParticleSystem::~tParticleSystem(void)
{
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
			particle.vertex_data.size = 0.05;

			dir.Normalize();
			particle.velocity = RandomFloat(velocity_min, velocity_max) * dir;
			particles.push_back(particle);
		}
	}


	int particles_count = particles.size();
	for(int i=0; i<particles_count; i++)
	{
		tParticle &particle = particles[i];

		if(gravity_enabled)
			particle.velocity += gravity * time;

		particle.vertex_data.position += time * particle.velocity;

		tVector dir = particle.vertex_data.position - center;
		if(abs(dir.x) > max_dist || abs(dir.y) > max_dist || abs(dir.z) > max_dist)
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

		tVector cam_pos = renderer->GetCamera()->GetPosition();
		tVector cam_dir = renderer->GetCamera()->GetDirection();

		for(vector<tParticle>::iterator pi=particles.begin(); pi!=particles.end(); pi++)
		{
			tParticle &particle = *pi;
			particle.compare = Dot(particle.vertex_data.position - cam_pos, cam_dir);
		}

		sort(particles.begin(), particles.end(), ParticleCompare());
	}
	else // if(blend_type == ADD)
	{
		glBlendFunc(GL_ONE, GL_ONE);
		glDepthMask(GL_FALSE);
	}

	vertex_data.resize(particles_count * 4);
	int i=0;
	float *data = vertex_data.data();
	for(vector<tParticle>::iterator pi=particles.begin(); pi!=particles.end(); pi++)
	{
		memcpy(data + i, &(*pi).vertex_data, sizeof(float) * 4);
		i+=4;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	renderer->GetParticleShader()->SetColor(Vec(1.0, 0.5, 0.1), 1.0);
	renderer->GetParticleShader()->SetTexture(tex);

	vao->Draw(GL_POINTS, 0, particles_count);

	tVAO::UnBind();
}

