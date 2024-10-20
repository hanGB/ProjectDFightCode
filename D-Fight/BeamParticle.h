#pragma once
#include "GravityParticle.h"

class CBeamParticle : public CGravityParticle {
public:
	CBeamParticle();
	~CBeamParticle();

private:
	glm::vec3 m_vRotation;

public:
	void SetRotation(glm::vec3 rotate);
	virtual void CreateVAO();

private:

};