#pragma once
#include "GravityParticle.h"

class CLandingDustEffectParticle : public CGravityParticle {
public:
	CLandingDustEffectParticle();
	~CLandingDustEffectParticle();

private:
	float m_fRadius = 2.0f;

public:
	virtual void CreateVAO();

	void SetRadius(float radius);

private:

};