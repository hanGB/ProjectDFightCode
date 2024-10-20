#include "stdafx.h"
#include "BloodEffectParticle.h"
#include <random>

CBloodEffectParticle::CBloodEffectParticle()
{

}

CBloodEffectParticle::~CBloodEffectParticle()
{
}

void CBloodEffectParticle::SetDirection(glm::vec3 dir)
{
	m_vDirection = glm::normalize(dir);
}

void CBloodEffectParticle::CreateVAO()
{
	int particleVertexCount = 4 * 6 * 10 * 10;
	SetVertexCount(particleVertexCount);

	float* vertices = new float[particleVertexCount];
	float* velocity = new float[particleVertexCount];
	float* sizeAndLifeTime = new float[particleVertexCount];	// 크기 x, y, 생존 시간, 시작 시간

	// uv 좌표
	int index = 3;
	int indexForVelocity = 3;
	for (int angle = 0; angle < particleVertexCount / 24 * 36; angle += 36) {
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
	for (int angle = 0; angle < particleVertexCount / 24 * 36; angle += 36) {
		float fAngle = (float)angle;
		for (int t = 0; t < 6; ++t) {
			vertices[index] = glm::cos(glm::radians(fAngle)) * 0.1f;
			index++;
			vertices[index] = 0.0f;
			index++;
			vertices[index] = glm::sin(glm::radians(fAngle)) * 0.1f;

			index++;
			index++;
		}
	}

	std::default_random_engine dre;
	std::uniform_real_distribution<float> urd(0.0f, 0.2f);
	std::uniform_real_distribution<float> urdScale(0.5f, 10.0f);

	// 시작 속도
	index = 0;
	for (int angle = 0; angle < particleVertexCount / 24 * 36; angle += 36) {
		float xScale = urdScale(dre);
		float yScale = urdScale(dre);
		float zScale = urdScale(dre);

		for (int t = 0; t < 6; ++t) {
			velocity[index] = m_vDirection.x * xScale;
			index++;
			velocity[index] = m_vDirection.y * yScale;
			index++;
			velocity[index] = m_vDirection.z * zScale;
			index++;
			index++;
		}
	}

	index = 0;
	for (int angle = 0; angle < particleVertexCount / 24 * 36; angle += 36) {
		float st = urd(dre);
		float lt = st + 10.0f;

		for (int t = 0; t < 6; ++t) {
			index += 2;
			sizeAndLifeTime[index] = lt;
			index++;
			sizeAndLifeTime[index] = st;
			index++;
		}
	}

	std::uniform_real_distribution<float> urdSize(0.5f, 1.5f);

	index = 0;
	for (int angle = 0; angle < particleVertexCount / 24 * 36; angle += 36) {
		float size = urdSize(dre);
		for (int t = 0; t < 6; ++t) {
			sizeAndLifeTime[index] = 0.2f * size;
			index++;
			sizeAndLifeTime[index] = 0.3f * size;
			index++;
			index += 2;
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particleVertexCount / 2, sizeAndLifeTime, GL_STATIC_DRAW);

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
