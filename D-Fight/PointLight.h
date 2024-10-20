#pragma once
#include "GameLight.h"

class CPointLight : public CGameLight {
public:
	CPointLight(glm::vec3 pos, glm::vec4 color);
	~CPointLight();

private:
	glm::vec3 m_vPosition;

	float m_fConstant;
	float m_fLinear;
	float m_fQuadratic;

	// 복수의 포인트 라이트를 만들기 위한 변수
	glm::vec3* m_vPositions = NULL;
	int* m_bIsPointLightOn = NULL; // uniform은 bool값의 배열을 정확하게 받을 수 없음으로 int로 대신 사용
	int m_iNumOfPointLights = 0;

public:
	void UseLight(int shaderType);

	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 pos);

	void SetAttenuation(float constant, float linear, float quadratic);

	void SetPointLights(glm::vec3* pos, int num);
	void SetIsPointLightsOn(bool on);
};
