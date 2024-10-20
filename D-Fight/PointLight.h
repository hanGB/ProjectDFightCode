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

	// ������ ����Ʈ ����Ʈ�� ����� ���� ����
	glm::vec3* m_vPositions = NULL;
	int* m_bIsPointLightOn = NULL; // uniform�� bool���� �迭�� ��Ȯ�ϰ� ���� �� �������� int�� ��� ���
	int m_iNumOfPointLights = 0;

public:
	void UseLight(int shaderType);

	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 pos);

	void SetAttenuation(float constant, float linear, float quadratic);

	void SetPointLights(glm::vec3* pos, int num);
	void SetIsPointLightsOn(bool on);
};
