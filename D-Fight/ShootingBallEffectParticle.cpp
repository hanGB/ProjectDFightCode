#include "stdafx.h"
#include "ShootingBallEffectParticle.h"
#include <random>

CShootingBallEffectParticle::CShootingBallEffectParticle()
{
	SetIsApplyGravity(false);
}

CShootingBallEffectParticle::~CShootingBallEffectParticle()
{
}

void CShootingBallEffectParticle::SetDirection(glm::vec3 dir)
{
	m_vDirection = dir;
}

void CShootingBallEffectParticle::CreateVAO()
{
	int particleVertexCount = 4 * 6 * 73;
	SetVertexCount(particleVertexCount);

	float* vertices = new float[particleVertexCount];
	float* velocity = new float[particleVertexCount];
	float* sizeAndLifeTime = new float[particleVertexCount];	// 크기 x, y, 생존 시간, 시작 시간

	// uv 좌표
	int index = 3;
	int indexForVelocity = 3;
	for (int t = 0; t < particleVertexCount / 24; ++t) {
		// 0
		vertices[index] = 0.f;
		velocity[indexForVelocity] = 1.f;
		index += 4;
		indexForVelocity += 4;

		// 1
		vertices[index] = 0.f;
		velocity[indexForVelocity] = 0.f;
		index += 4;
		indexForVelocity += 4;

		// 2
		vertices[index] = 1.f;
		velocity[indexForVelocity] = 0.f;
		index += 4;
		indexForVelocity += 4;

		// 3
		vertices[index] = 0.f;
		velocity[indexForVelocity] = 1.f;
		index += 4;
		indexForVelocity += 4;

		// 4
		vertices[index] = 1.f;
		velocity[indexForVelocity] = 0.f;
		index += 4;
		indexForVelocity += 4;

		// 5
		vertices[index] = 1.f;
		velocity[indexForVelocity] = 1.f;
		index += 4;
		indexForVelocity += 4;
	}

	// 시작 위치
	index = 0;
	for (int t = 0; t < 6; ++t) {
		vertices[index] = 0.0f;
		index++;
		vertices[index] = 0.0f;
		index++;
		vertices[index] = 0.0f;
		index++;
		index++;
	}

	for (int i = 0; i < particleVertexCount / 24 - 1; ++i) {
		for (int t = 0; t < 6; ++t) {
			vertices[index] = m_vDirection.x;
			index++;
			vertices[index] = -1.5f;
			index++;
			vertices[index] = m_vDirection.z;
			index++;
			index++;
		}
	}

	std::default_random_engine dre;
	std::uniform_real_distribution<float> urd(-2.0f, 2.0f);

	// 시작 속도
	index = 0;
	for (int t = 0; t < 6; ++t) {
		velocity[index] = m_vDirection.x * 2;
		index++;
		velocity[index] = m_vDirection.y * 2;
		index++;
		velocity[index] = m_vDirection.z * 2;
		index++;
		index++;
	}
	for (int i = 0; i < particleVertexCount / 24 - 1; ++i) {
		float randX = urd(dre);
		float randY = urd(dre);
		float randZ = urd(dre);
		for (int t = 0; t < 6; ++t) {
			velocity[index] = randX;
			index++;
			velocity[index] = randY;
			index++;
			velocity[index] = randZ;
			index++;
			index++;
		}
	}

	index = 0;
	for (int t = 0; t < 6; ++t) {
		sizeAndLifeTime[index] = 0.7f;
		index++;
		sizeAndLifeTime[index] = 0.7f;
		index++;
		index += 2;
	}
	for (int i = 0; i < particleVertexCount / 24 - 1; ++i) {
		for (int t = 0; t < 6; ++t) {
			sizeAndLifeTime[index] = 0.1f;
			index++;
			sizeAndLifeTime[index] = 0.1f;
			index++;
			index += 2;
		}
	}
	index = 0;
	for (int t = 0; t < 6; ++t) {
		index += 2;
		sizeAndLifeTime[index] = 0.7f;
		index++;
		sizeAndLifeTime[index] = 0.2f;
		index++;
	}
	for (int i = 0; i < particleVertexCount / 24 - 1; ++i) {
		for (int t = 0; t < 6; ++t) {
			index += 2;
			sizeAndLifeTime[index] = 0.5f;
			index++;
			sizeAndLifeTime[index] = 0.9f;
			index++;
		}
	}

	// 정점 배열 생성
	GLuint VAO;

	GLuint vertexVBO;
	GLuint velocityVBO;
	GLuint lifeTimeVBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &velocityVBO);
	glGenBuffers(1, &lifeTimeVBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particleVertexCount, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velocityVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particleVertexCount, velocity, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, lifeTimeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particleVertexCount, sizeAndLifeTime, GL_STATIC_DRAW);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &velocityVBO);
	glDeleteBuffers(1, &lifeTimeVBO);

	delete[] vertices;
	delete[] velocity;
	delete[] sizeAndLifeTime;

	SetVAO(VAO);
}
