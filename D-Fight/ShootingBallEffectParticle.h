#pragma once
#include "GravityParticle.h"

class CShootingBallEffectParticle : public CGravityParticle {
public:
	CShootingBallEffectParticle();
	~CShootingBallEffectParticle();

private:
	glm::vec3 m_vDirection;

public:
	void SetDirection(glm::vec3 dir);

	virtual void CreateVAO();

private:

};