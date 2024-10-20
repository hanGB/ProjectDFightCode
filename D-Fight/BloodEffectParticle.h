#pragma once
#include "GravityParticle.h"

class CBloodEffectParticle : public CGravityParticle {
public:
	CBloodEffectParticle();
	~CBloodEffectParticle();

private:
	glm::vec3 m_vDirection;

public:
	void SetDirection(glm::vec3 dir);
	virtual void CreateVAO();

private:

};