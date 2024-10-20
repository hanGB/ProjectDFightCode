#pragma once
#include "GameLight.h"

class CSpotLight : public CGameLight {
public:
	CSpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec4 color);
	~CSpotLight();

private:
	glm::vec3 m_vDirection;
	glm::vec3 m_vPosition;

	float m_fCutOff;
	float m_fOutherCutOff;

	float m_fDistanceBias;

public:
	void UseLight(int shaderType);

	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 pos);
	
	glm::vec3 GetDirection() const;
	void SetDirection(glm::vec3 dir);

	void SetCutOff(float cutOff);
};