
#include "towerengine.h"

#include <list>

using namespace std;

tParticleSystem::tParticleSystem(tVector center, float radius, float velocity_min, float velocity_max, float spawn_time)
{
	this->center = center;
	this->radius = radius;
	this->velocity_min = velocity_min;
	this->velocity_max = velocity_max;
	this->spawn_time = spawn_time;

	last_spawn_time = 0.0;

	vao = new tVAO();
	vao->Bind();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(tShader::vertex_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	tVAO::UnBind();
}

tParticleSystem::~tParticleSystem(void)
{
	for(list<tParticle *>::iterator pi=particles.begin(); pi!=particles.end(); pi++)
		delete *pi;
}

void tParticleSystem::Step(float time)
{
	last_spawn_time += time;

	if(last_spawn_time >= spawn_time)
	{
		int spawn_count = (int)(last_spawn_time / spawn_time);
		last_spawn_time -= (float)spawn_count * spawn_time;

		tParticle *particle;
		tVector dir;

		for(int i=0; i<spawn_count; i++)
		{
			dir = Vec(RandomFloat(-1.0, 1.0), RandomFloat(-1.0, 1.0), RandomFloat(-1.0, 1.0));
			dir.Normalize();

			particle = new tParticle();
			particle->position = center + dir * radius * RandomFloat(0.0, 1.0);
			particle->velocity = RandomFloat(velocity_min, velocity_max) * dir;
			particles.push_back(particle);
		}
	}

	for(list<tParticle *>::iterator pi=particles.begin(); pi!=particles.end(); pi++)
	{
		(*pi)->position += time * (*pi)->velocity;
	}
}

void tParticleSystem::Render(tRenderer *renderer)
{
	int particles_count = particles.size();

	//if(particles_count == 0)
	//	return;

	vertex_data.resize(particles_count * 3);
	int i=0;
	//float *data = vertex_data.data();
	for(list<tParticle *>::iterator pi=particles.begin(); pi!=particles.end(); pi++)
	{
		//memcpy
		vertex_data[i  ] = (*pi)->position.x;
		vertex_data[i+1] = (*pi)->position.y;
		vertex_data[i+2] = (*pi)->position.z;
		i+=3;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	/*tShader::Unbind();
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glPointSize(6.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultTransposeMatrixf(renderer->GetCamera()->GetProjectionMatrix().GetData());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultTransposeMatrixf(renderer->GetCamera()->GetModelViewMatrix().GetData());*/

	renderer->GetParticleShader()->SetColor(Vec(1.0, 1.0, 0.0), 1.0);

	vao->Draw(GL_POINTS, 0, particles_count);

	tVAO::UnBind();
}

