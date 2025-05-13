#include "Particle.h"


Particle::Particle()
{

}

Particle::Particle(int amount, glm::vec3 shotPosition)
	:delteTime(0.0f)
{
	this->amount = amount;
	this->shotPosition = shotPosition;
	this->Init();
}

void Particle::Update(float dt, unsigned int newParticles, glm::vec3 offset)
{
	for (unsigned int i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], offset);
	}
	// update all particles
	for (unsigned int i = 0; i < this->amount; ++i)
	{
		randomdir = glm::vec3(
			-(rand() % 2000 - 1000.0f) / 1000.0f,
			-(rand() % 2000 - 1000.0f) / 1000.0f,
			-(rand() % 2000 - 1000.0f) / 1000.0f
		);
		ParticleData& pd = this->particles[i];
		pd.life -= dt; // reduce life
		if (pd.life > 0.0f)
		{	// particle is alive, thus update
			pd.Velocity += delteTime * 0.05f;
			pd.position += pd.Velocity * dt;
		}
	}
}

void Particle::Draw(DrawShader& shader)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (ParticleData particle : this->particles)
	{
		if (particle.life > 0.0f)
		{
			shader.setUniformVec3("particlePosition", particle.position);
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

unsigned int lastUsedParticle = 0;
unsigned int Particle::firstUnusedParticle()
{
	for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// otherwise, do a linear search
	for (unsigned int i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// 没有找到消亡的粒子，那么就淘汰一号位的粒子
	lastUsedParticle = 0;
	return 0;
}

void Particle::respawnParticle(ParticleData& particle, glm::vec3 offset)
{
	particle.position = shotPosition;
	particle.life = 1.0f;
	particle.Velocity = maindir + randomdir * spread;
}

void Particle::Init()
{
	float particle_quad[] = {

		0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		1.0f, 0.0f,	0.0f,	1.0f, 0.0f,
		1.0f, 1.0f,	0.0f,	1.0f, 1.0f,
					
		1.0f, 1.0f,	0.0f,	1.0f, 1.0f,
		0.0f, 1.0f,	0.0f,	0.0f, 1.0f,
		0.0f, 0.0f,	0.0f,	0.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glBindVertexArray(this->VAO);
	// fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	// create this->amount default particle instances
	for (unsigned int i = 0; i < this->amount; ++i)
		this->particles.push_back(ParticleData());
}
