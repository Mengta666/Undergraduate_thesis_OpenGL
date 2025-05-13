#pragma once

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>

#include <vector>

#include "DrawShader.h"
#include "Texture.h"

struct ParticleData
{
	glm::vec3 position;
	glm::vec3 Velocity;
	float life;

	ParticleData() : position(0.0f), Velocity(0.0f), life(0.0f) {};								// ³õÊ¼»¯

};

class Particle
{
public:
	Particle();
	Particle(int amount, glm::vec3 shotPosition);

	void Update(float delteTime , unsigned int newParticles, glm::vec3 offset = glm::vec3(0.0f));

	void Draw(DrawShader& shader);

	inline void setDelteTime(float dt) { this->delteTime = dt; };

private:
	GLuint VBO, VAO;

	unsigned int amount;
	float delteTime;
	std::vector<ParticleData> particles;

	glm::vec3 shotPosition;

	float spread = 1.5f;
	glm::vec3 maindir = glm::vec3(0.0f, -10.0f, 0.0f);
	glm::vec3 randomdir = glm::vec3(0.0f);

	unsigned int firstUnusedParticle();

	void respawnParticle(ParticleData& particle, glm::vec3 offset = glm::vec3(0.0f));

	void Init();

};
