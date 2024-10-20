#include "stdafx.h"
#include "BeamParticle.h"
#include <random>

CBeamParticle::CBeamParticle()
{
	SetIsApplyGravity(false);
}

CBeamParticle::~CBeamParticle()
{

}

void CBeamParticle::SetRotation(glm::vec3 rotate)
{
	m_vRotation = rotate;
}

void CBeamParticle::CreateVAO()
{
	int particleVertexCount = 4 * 6 * 20 * 50;
	SetVertexCount(particleVertexCount);

	float* vertices = new float[particleVertexCount];
	float* velocity = new float[particleVertexCount];
	float* sizeAndLifeTime = new float[particleVertexCount];	// 크기 x, y, 생존 시간, 시작 시간

	// uv 좌표
	int index = 3;
	int indexForVelocity = 3;
	for (int angle = 0; angle < particleVertexCount / 24 * 18; angle += 18) {
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

	glm::quat modelQuat
		= glm::quat(glm::vec3(glm::radians(m_vRotation)));
	glm::mat4 rotationMat = glm::toMat4(modelQuat);

	std::default_random_engine dre;
	std::uniform_real_distribution<float> urd(0.5f, 1.0f);

	// 시작 위치
	index = 0;
	for (int angle = 0; angle < particleVertexCount / 24 * 18; angle += 18) {
		float fAngle = (float)angle;
		float dis = fAngle / 360.0f;
		float size = urd(dre);
		for (int t = 0; t < 6; ++t) {
			glm::vec3 pos = rotationMat * glm::vec4(
				glm::cos(glm::radians(fAngle)) * 0.35f,
				glm::sin(glm::radians(fAngle)) * 0.35f,
				-dis / 4.0f, 1.0f);

			vertices[index] = pos.x * size;
			index++;
			vertices[index] = pos.y * size;
			index++;
			vertices[index] = pos.z;

			index++;
			index++;
		}
	}

	// 시작 속도
	index = 0;
	for (int angle = 0; angle < particleVertexCount / 24 * 18; angle += 18) {
		for (int t = 0; t < 6; ++t) {
			velocity[index] = 0.f;
			index++;
			velocity[index] = 0.f;
			index++;
			velocity[index] = 0.f;
			index++;
			index++;
		}
	}

	index = 0;
	for (int angle = 0; angle < particleVertexCount / 24 * 18; angle += 18) {
		float fAngle = (float)angle;
		float dis = fAngle / 360.0f;
		float st = dis / 300.0f;
		float lt = 1.0f;
		for (int t = 0; t < 6; ++t) {
			index += 2;
			sizeAndLifeTime[index] = lt;
			index++;
			sizeAndLifeTime[index] = st;
			index++;
		}
	}
	index = 0;
	for (int angle = 0; angle < particleVertexCount / 24 * 18; angle += 18) {
		for (int t = 0; t < 6; ++t) {
			sizeAndLifeTime[index] = 0.2f;
			index++;
			sizeAndLifeTime[index] = 0.2f;
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
