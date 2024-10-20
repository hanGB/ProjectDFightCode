#pragma once
#include "GravityParticle.h"

class CFountainWaterDropParticle : public CGravityParticle {
public:
	CFountainWaterDropParticle();
	~CFountainWaterDropParticle();

private:
	float m_fRadiusTop;
	float m_fRadiusButtom;

public:
	void SetRadiusTopAndButtom(float top, float buttom);
	virtual void CreateVAO();

private:

};